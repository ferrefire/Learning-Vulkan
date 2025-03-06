#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

struct TreeRenderData
{
	uint posxz;
	uint posyroty;
	uint scaxcoly;
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
	uint treeLod4RenderBase;
	uint treeLod4RenderCount;
	uint treeLod5RenderBase;
	uint treeLod5RenderCount;
	uint treeTotalRenderBase;
	uint treeTotalRenderCount;
	float spacing;
	float spacingMult;
	//uint leafCountTotal;
	//uint leafCount0;
	//uint leafCount1;
	//uint leafCount2;
	//uint leafCount3;
	//uint leafCount4;
	vec4 leafCounts[6];
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
layout(location = 2) out vec3 worldPosition;
//layout(location = 3) out vec4 shadowPositions[CASCADE_COUNT];

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

void main()
{
    vec3 position = vec3(0.0);
	float rotation = 0.0;
	float scale = 1.0;
	float color = 1.0;

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
	color = scaxcoly.y;

    position += variables.viewPosition;

	vec3 scaledPosition = inPosition * scale;

    //rotation = 0.0;

	mat4 rotationMatrix = GetRotationMatrix(radians(rotation), vec3(0.0, 1.0, 0.0));
    vec3 objectPosition = (rotationMatrix * vec4(scaledPosition, 1.0)).xyz;
    normal = (rotationMatrix * vec4(inNormal, 0.0)).xyz;

	coord = inCoordinate;
    //coord.x = 1.0 - coord.x;
    coord.y = 1.0 - coord.y;

    worldPosition = ObjectToWorld(objectPosition, mat4(1)) + position;

    gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);

	//for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);
}