#version 450 core

#extension GL_ARB_shading_language_include : require

layout(set = 0, binding = 0) uniform Variables 
{
    vec3 viewPosition;
	vec4 resolution;
} variables;

layout(set = 1, binding = 0) uniform ObjectData 
{
    mat4 model;
    mat4 view;
    mat4 projection;
} objectData;

layout(triangles, fractional_odd_spacing, cw) in;

layout(location = 0) out vec3 worldPosition;

#include "transformation.glsl"
#include "heightmap.glsl"

void main()
{
    vec4 position = gl_in[0].gl_Position * gl_TessCoord[0] + gl_in[1].gl_Position * gl_TessCoord[1] + gl_in[2].gl_Position * gl_TessCoord[2];

	worldPosition = position.xyz;
	//worldPosition.y = ObjectToWorld(vec3(0)).y + SampleDynamic(worldPosition.xz) * heightMapHeight;
	//worldPosition.y = ObjectToWorld(vec3(0)).y + texture(heightMapSampler, vec2(worldPosition.xz * 0.0001 + 0.5)).r * 5000;
	worldPosition.y = ObjectToWorld(vec3(0)).y + SampleDynamic(worldPosition.xz) * 5000;

	gl_Position = objectData.projection * objectData.view * vec4(worldPosition, 1.0);
}