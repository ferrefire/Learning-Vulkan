#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 terrainNormal;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"

void main()
{
	vec3 diffuse = DiffuseLighting(terrainNormal, vec3(1));

	outColor = vec4(diffuse, 1.0);
}