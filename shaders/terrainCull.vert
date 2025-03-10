#version 450

#extension GL_ARB_shading_language_include : require

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

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"

void main()
{
	vec3 worldPosition = ObjectToWorld(inPosition, objectDatas[pc.chunkIndex].model);
	worldPosition.y += GetTerrainHeight(worldPosition.xz);

	gl_Position = vec4(worldPosition, 1.0);
}