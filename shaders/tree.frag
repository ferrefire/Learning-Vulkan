#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"

void main()
{
	//vec3 diffuse = DiffuseLighting(terrainNormal, vec3(1));
	//vec3 combinedColor = diffuse;

	//outColor = vec4(combinedColor, 1.0);
	outColor = vec4(1.0);
}