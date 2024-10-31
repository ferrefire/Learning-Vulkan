#version 450

#extension GL_ARB_shading_language_include : require

struct GrassData
{
    vec3 position;
    vec3 rotation;
    vec3 normal;
};

layout(std430, set = 1, binding = 0) buffer DataBuffer
{
	GrassData data[];
};

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 terrainNormal;

#include "variables.glsl"
#include "transformation.glsl"

void main()
{
	vec3 position = data[gl_InstanceIndex].position + variables.viewPosition;

	terrainNormal = data[gl_InstanceIndex].normal;
	worldPosition = ObjectToWorld(inPosition, mat4(1)) + position;
    gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);
}