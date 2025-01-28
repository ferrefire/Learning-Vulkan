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

//layout(location = 0) out vec3 outPosition;
//layout(location = 1) out vec2 outTexCoord;

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"

void main()
{
    //outTexCoord = vec2(inPosition.xz + 0.5);
    //vec3 position = inPosition;
    //position.y += texture(heightMapSampler, outTexCoord).r * 0.25;
    ////position.y += texture(heightMapSampler, vec2(inPosition.xz + 0.5)).r * 0.25;
	//outPosition = (objectData.model * vec4(position, 1.0)).xyz;
	////if (distance(outPosition.xz, variables.viewPosition.xz + vec2(0, 1000)) < 100) position.y += 1;
    //gl_Position = objectData.projection * objectData.view * objectData.model * vec4(position, 1.0);

	vec3 worldPosition = ObjectToWorld(inPosition, objectDatas[pc.chunkIndex].model);
	//worldPosition.y += texture(heightMapSampler, vec2(inPosition.xz + 0.5)).r * 5000;
	worldPosition.y += SampleDynamic(worldPosition.xz) * variables.terrainHeight;

	gl_Position = vec4(worldPosition, 1);
}