#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"

void main()
{
	vec3 diffuse = DiffuseLighting(inNormal, vec3(1));
	vec3 combinedColor = diffuse * 0.5;

	outColor = vec4(combinedColor, 1.0);
	//outColor = vec4(vec3(0.5), 1.0);
}