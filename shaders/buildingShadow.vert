#version 450

#extension GL_ARB_shading_language_include : require

struct BuildingData
{
	mat4 translation;
	mat4 orientation;
};

layout(set = 1, binding = 0) uniform BuildingBuffers
{
	BuildingData buildingData[250];
} buildingBuffers;

layout(push_constant, std430) uniform PushConstants
{
	uint shadowCascade;
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinate;
layout(location = 2) in vec3 inNormal;

#include "variables.glsl"

void main()
{
	int index = gl_InstanceIndex;

    vec3 objectPosition = inPosition;

    vec3 worldPosition = (buildingBuffers.buildingData[index].translation * buildingBuffers.buildingData[index].orientation * vec4(objectPosition, 1.0)).xyz;

    worldPosition -= variables.terrainOffset;

    gl_Position = variables.shadowCascadeMatrix[pc.shadowCascade] * vec4(worldPosition, 1.0);
}