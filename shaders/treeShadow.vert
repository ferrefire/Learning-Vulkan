#version 450

#extension GL_ARB_shading_language_include : require

struct TreeRenderData
{
	uint posxz;
	uint posyroty;
	uint scaxcoly;
};

layout(std430, set = 1, binding = 0) readonly buffer RenderBuffer
{
	TreeRenderData renderData[];
};

layout(set = 1, binding = 1) uniform TreeVariables
{
	uint treeBase;
	uint treeCount;
	uint treeLod0RenderBase;
	uint treeLod0RenderCount;
	uint treeLod1RenderBase;
	uint treeLod1RenderCount;
	uint treeLod2RenderBase;
	uint treeLod2RenderCount;
	uint treeLod3RenderBase;
	uint treeLod3RenderCount;
	uint treeLod4RenderBase;
	uint treeLod4RenderCount;
	uint treeLod5RenderBase;
	uint treeLod5RenderCount;
	uint treeTotalRenderBase;
	uint treeTotalRenderCount;
	float spacing;
	float spacingMult;
	vec4 leafCounts[6];
} treeVariables;

layout(push_constant, std430) uniform PushConstants
{
    uint treeLod;
    uint shadowCascade;
} pc;

layout(set = 0, binding = 8) uniform sampler2D windSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinate;
//layout(location = 2) in vec3 inNormal;

//layout(location = 0) out vec2 coord;
//layout(location = 1) out vec3 normal;

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

void main()
{
    vec3 position = vec3(0.0);
	float rotation = 0.0;
	float scale = 1.0;
	//float color = 1.0;

	uint dataIndex = gl_InstanceIndex;
	if (pc.treeLod >= 1) dataIndex += treeVariables.treeLod0RenderCount;
	if (pc.treeLod >= 2) dataIndex += treeVariables.treeLod1RenderCount;
	if (pc.treeLod >= 3) dataIndex += treeVariables.treeLod2RenderCount;
	if (pc.treeLod >= 4) dataIndex += treeVariables.treeLod3RenderCount;
	if (pc.treeLod >= 5) dataIndex += treeVariables.treeLod4RenderCount;

	position.xz = unpackHalf2x16(renderData[dataIndex].posxz);
	vec2 posyroty = unpackHalf2x16(renderData[dataIndex].posyroty);
	position.y = posyroty.x;
	rotation = posyroty.y * 360.0;
	vec2 scaxcoly = unpackHalf2x16(renderData[dataIndex].scaxcoly);
	scale = scaxcoly.x;
	//color = scaxcoly.y;

	//position = renderData[dataIndex].position;
	//rotation = renderData[dataIndex].rotscacol.x * 360.0;
	//scale = renderData[dataIndex].rotscacol.y;
	//color = renderData[dataIndex].rotscacol.z;

    position += variables.viewPosition;

	mat4 rotationMatrix = GetRotationMatrix(radians(rotation), vec3(0.0, 1.0, 0.0));
    vec3 objectPosition = (rotationMatrix * vec4(inPosition  * scale, 1.0)).xyz;
    //normal = (rotationMatrix * vec4(inNormal, 0.0)).xyz;

	//coord = inCoordinate;
    vec3 worldPosition = ObjectToWorld(objectPosition, mat4(1)) + position;

	if (pc.treeLod <= 1)
	{
		vec2 windUV = (worldPosition.xz + variables.terrainOffset.xz) * variables.windDistanceMult;
		float wave = 1.0 - textureLod(windSampler, windUV, 0).r;
		worldPosition.xz += wave * inCoordinate.x * variables.windStrength;
	}

    //gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);
    //gl_Position = variables.shadowLod1Matrix * vec4(worldPosition, 1.0);
    //gl_Position = variables.shadowLod1Matrix * variables.shadowLod1Projection * variables.shadowLod1View * vec4(worldPosition, 1.0);

	gl_Position = variables.shadowCascadeMatrix[pc.shadowCascade] * vec4(worldPosition, 1.0);
}