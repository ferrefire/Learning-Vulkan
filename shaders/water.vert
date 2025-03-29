#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec4 shadowPositions[CASCADE_COUNT];

#include "variables.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"

void main()
{
	worldPosition = inPosition * 50000.0;
	worldPosition.y = -variables.terrainOffset.y + variables.waterHeight.x - 25.0;

	for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);

	gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}