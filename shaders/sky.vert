#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinates;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outCoordinates;

#include "variables.glsl"

void main()
{
    outCoordinates = inCoordinates;
	outPosition = inPosition;
	//outPosition = (variables.view * vec4(inPosition, 0)).xyz;

    gl_Position = vec4((inCoordinates * 2.0 - 1.0), 1.0, 1.0);
}