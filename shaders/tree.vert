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
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 normal;

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
	rotation = yy.y * 360.0;
	vec2 xx = unpackHalf2x16(renderData[gl_InstanceIndex].scaxcoly);
	scale = xx.x;
	color = xx.y;

    position += variables.viewPosition;

	mat4 rotationMatrix = GetRotationMatrix(radians(rotation), vec3(0.0, 1.0, 0.0));
    vec3 objectPosition = (rotationMatrix * vec4(inPosition, 1.0)).xyz;
    normal = (rotationMatrix * vec4(inNormal, 0.0)).xyz;

    //vec3 worldPosition = ObjectToWorld(inPosition * vec3(1.5, 15, 1.5), mat4(1)) + position + vec3(0, 7.5, 0);
    vec3 worldPosition = ObjectToWorld(objectPosition, mat4(1)) + position;

	//normal = inNormal;
    gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);
}