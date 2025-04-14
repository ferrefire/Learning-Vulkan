#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

struct BuildingData
{
	mat4 translation;
	mat4 orientation;
};

layout(set = 1, binding = 0) uniform BuildingBuffers
{
	BuildingData buildingData[250];
} buildingBuffers;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinate;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 objectPosition;
layout(location = 1) out vec3 worldPosition;
layout(location = 2) out vec2 outCoordinate;
layout(location = 3) out vec3 objectNormal;
layout(location = 4) out vec3 worldNormal;
layout(location = 5) out mat4 orientation;
layout(location = 9) out flat ivec2 type;
layout(location = 10) out vec4 shadowPositions[CASCADE_COUNT];

#include "variables.glsl"

void main()
{
	int index = gl_InstanceIndex;

	orientation = buildingBuffers.buildingData[index].orientation;

    objectNormal = inNormal;
    worldNormal = (buildingBuffers.buildingData[index].orientation * vec4(inNormal, 0.0)).xyz;

	outCoordinate = inCoordinate;

	type = ivec2(int(inColor.x), int(inColor.y));

    objectPosition = inPosition;

    worldPosition = (buildingBuffers.buildingData[index].translation * buildingBuffers.buildingData[index].orientation * vec4(objectPosition, 1.0)).xyz;

    worldPosition -= variables.terrainOffset;

    for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);

    gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}