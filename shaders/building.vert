#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinate;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inColor;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec2 outCoordinate;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out flat ivec2 type;
layout(location = 4) out vec4 shadowPositions[CASCADE_COUNT];

#include "variables.glsl"

void main()
{
    outNormal = inNormal;

	outCoordinate = inCoordinate;

	type = ivec2(int(inColor.x), int(inColor.y));

    vec3 objectPosition = inPosition;

    worldPosition = objectPosition + vec3(0.0, 2500.0, 0.0);

    worldPosition -= variables.terrainOffset;

    for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);

    gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}