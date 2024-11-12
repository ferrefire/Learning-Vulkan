#version 450

#extension GL_ARB_shading_language_include : require

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

layout(location = 0) in vec3 inPosition;

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

void main()
{
    vec3 position = vec3(0.0);
	float rotation = 0.0;
	float scale = 1.0;
	float color = 1.0;

    position.xz = unpackHalf2x16(renderData[gl_InstanceIndex].posxz);
	vec2 yy = unpackHalf2x16(renderData[gl_InstanceIndex].posyroty);
	position.y = yy.x;
	rotation = yy.y;
	vec2 xx = unpackHalf2x16(renderData[gl_InstanceIndex].scaxcoly);
	scale = xx.x;
	color = xx.y;

    position += variables.viewPosition;

    vec3 worldPosition = ObjectToWorld(inPosition * vec3(1.5, 15, 1.5), mat4(1)) + position + vec3(0, 7.5, 0);

    gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);
}