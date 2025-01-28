#version 450 core

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

layout(triangles, fractional_odd_spacing, cw) in;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec4 shadowPositions[CASCADE_COUNT];
//layout(location = 1) out vec4 shadowLod0Position;
//layout(location = 2) out vec4 shadowLod1Position;
//layout(location = 1) out mat3 tbn;

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"
//#include "lighting.glsl"

void main()
{
    vec4 position = gl_in[0].gl_Position * gl_TessCoord[0] + gl_in[1].gl_Position * gl_TessCoord[1] + gl_in[2].gl_Position * gl_TessCoord[2];

	worldPosition = position.xyz;
	worldPosition.y = ObjectToWorld(vec3(0), objectDatas[pc.chunkIndex].model).y + SampleDynamic(worldPosition.xz) * variables.terrainHeight;

	for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);
	

	gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}