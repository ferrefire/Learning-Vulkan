#version 450 core

#extension GL_ARB_shading_language_include : require

#define OBJECT_DATA_COUNT 25
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
//layout(location = 1) out mat3 tbn;

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"
//#include "lighting.glsl"

void main()
{
    vec4 position = gl_in[0].gl_Position * gl_TessCoord[0] + gl_in[1].gl_Position * gl_TessCoord[1] + gl_in[2].gl_Position * gl_TessCoord[2];

	worldPosition = position.xyz;
	//worldPosition.y = ObjectToWorld(vec3(0)).y + SampleDynamic(worldPosition.xz) * heightMapHeight;
	//worldPosition.y = ObjectToWorld(vec3(0)).y + texture(heightMapSampler, vec2(worldPosition.xz * 0.0001 + 0.5)).r * 5000;
	worldPosition.y = ObjectToWorld(vec3(0), objectDatas[pc.chunkIndex].model).y + SampleDynamic(worldPosition.xz) * 5000;

	//vec3 terrainNormal = SampleNormalDynamic(worldPosition.xz, 1.0);
	//vec3 tangent = NormalToTangent(terrainNormal);
	//tbn = mat3(tangent, cross(terrainNormal, tangent) * -1, terrainNormal);

	gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);
}