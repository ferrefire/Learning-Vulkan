#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec2 outCoordinates;

#include "variables.glsl"

void main()
{
    outCoordinates = inPosition.xy + 0.5;

    gl_Position = vec4((inPosition.xy + vec2(0.5, -0.5)), 0.0, 1.0);
}