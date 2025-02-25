#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

#ifndef OBJECT_DATA_COUNT
#define OBJECT_DATA_COUNT 49
#endif

layout(set = 1, binding = 0) uniform ObjectData
{
    mat4 model;
} objectDatas[OBJECT_DATA_COUNT];

layout(push_constant, std430) uniform PushConstants
{
    uint chunkIndex;
} pc;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec4 shadowPositions[CASCADE_COUNT];

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"

void main()
{
	worldPosition = ObjectToWorld(inPosition, objectDatas[pc.chunkIndex].model);
	worldPosition.y += GetTerrainHeight(worldPosition.xz);

	for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);

	//gl_Position = variables.viewLodMatrix * vec4(worldPosition, 1.0);
	gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}