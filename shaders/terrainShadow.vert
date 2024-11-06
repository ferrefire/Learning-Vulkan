#version 450

#extension GL_ARB_shading_language_include : require

#define OBJECT_DATA_COUNT 49
layout(set = 1, binding = 0) uniform ObjectData
{
    mat4 model;
} objectDatas[OBJECT_DATA_COUNT];

layout(push_constant, std430) uniform PushConstants
{
    uint chunkIndex;
} pc;

layout(location = 0) in vec3 inPosition;

//layout(location = 0) out vec3 outPosition;
//layout(location = 1) out vec2 outTexCoord;

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"

void main()
{
	vec3 worldPosition = ObjectToWorld(inPosition, objectDatas[pc.chunkIndex].model);
	worldPosition.y += SampleDynamic(worldPosition.xz) * 5000;

	gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);
}