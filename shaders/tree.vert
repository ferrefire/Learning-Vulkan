#version 450

#extension GL_ARB_shading_language_include : require

struct TreeRenderData
{
	//uint posxz;
	//uint posyroty;
	//uint scaxcoly;
	vec3 position;
	vec3 rotscacol;
};

layout(std430, set = 1, binding = 0) buffer RenderBuffer
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
	uint treeTotalRenderBase;
	uint treeTotalRenderCount;
	float spacing;
	float spacingMult;
} treeVariables;

layout(push_constant, std430) uniform PushConstants
{
    uint treeLod;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinate;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec2 coord;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec4 shadowPosition;
layout(location = 3) out vec3 worldPosition;

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

void main()
{
    vec3 position = vec3(0.0);
	float rotation = 0.0;
	float scale = 1.0;
	float color = 1.0;

    //position.xz = unpackHalf2x16(renderData[gl_InstanceIndex].posxz);
	//vec2 yy = unpackHalf2x16(renderData[gl_InstanceIndex].posyroty);
	//position.y = yy.x;
	//rotation = yy.y * 360.0;
	//vec2 xx = unpackHalf2x16(renderData[gl_InstanceIndex].scaxcoly);
	//scale = xx.x;
	//color = xx.y;

	uint dataIndex = gl_InstanceIndex;
	if (pc.treeLod == 1) dataIndex += treeVariables.treeLod0RenderCount;
	else if (pc.treeLod == 2) dataIndex += treeVariables.treeLod1RenderCount;
	else if (pc.treeLod == 3) dataIndex += treeVariables.treeLod2RenderCount;

	position = renderData[dataIndex].position;
	rotation = renderData[dataIndex].rotscacol.x * 360.0;
	scale = renderData[dataIndex].rotscacol.y;
	color = renderData[dataIndex].rotscacol.z;

    position += variables.viewPosition;

	mat4 rotationMatrix = GetRotationMatrix(radians(rotation), vec3(0.0, 1.0, 0.0));
    vec3 objectPosition = (rotationMatrix * vec4(inPosition, 1.0)).xyz;
    normal = (rotationMatrix * vec4(inNormal, 0.0)).xyz;

	coord = inCoordinate;

	//if (gl_InstanceIndex == 0 && pc.treeLod == 0)
	//{
	//	position = vec3(-250, 750, -2050);
	//	objectPosition = inPosition;
	//	normal = inNormal;
	//}

    //vec3 worldPosition = ObjectToWorld(inPosition * vec3(1.5, 15, 1.5), mat4(1)) + position + vec3(0, 7.5, 0);
    worldPosition = ObjectToWorld(objectPosition, mat4(1)) + position;

	//normal = inNormal;
    gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);

	if (variables.shadowCascades == 1) shadowPosition = variables.shadowCascadeMatrix * vec4(worldPosition, 1.0);
	else shadowPosition = variables.shadowLod1Matrix * vec4(worldPosition, 1.0);
	//shadowPosition = variables.shadowLod0Matrix * vec4(worldPosition, 1.0);
}