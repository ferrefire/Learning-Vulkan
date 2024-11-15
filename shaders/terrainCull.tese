#version 450 core

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

layout(triangles, fractional_odd_spacing, cw) in;

//layout(location = 0) out vec3 worldPosition;
//layout(location = 1) out vec4 shadowPosition;
//layout(location = 1) out mat3 tbn;

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"
//#include "lighting.glsl"

void main()
{
    vec4 position = gl_in[0].gl_Position * gl_TessCoord[0] + gl_in[1].gl_Position * gl_TessCoord[1] + gl_in[2].gl_Position * gl_TessCoord[2];

	vec3 worldPosition = position.xyz;
	worldPosition.y = ObjectToWorld(vec3(0), objectDatas[pc.chunkIndex].model).y + SampleDynamic(worldPosition.xz) * 5000;

	//shadowPosition = variables.shadowProjection * variables.shadowView * vec4(worldPosition, 1.0);

	gl_Position = variables.cullMatrix * vec4(worldPosition, 1.0);
}