#version 450

#extension GL_ARB_shading_language_include : require

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
    vec3 objectPosition = inPosition;

    vec3 worldPosition = objectPosition + vec3(0.0, 2500.0, 0.0);

    worldPosition -= variables.terrainOffset;

    gl_Position = variables.shadowCascadeMatrix[pc.shadowCascade] * vec4(worldPosition, 1.0);
}