#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 outNormal;

#include "variables.glsl"

void main()
{
    outNormal = inNormal;

    vec3 objectPosition = inPosition;

    worldPosition = inPosition + vec3(0.0, 2500.0, 0.0);

    worldPosition -= variables.terrainOffset;

    gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}