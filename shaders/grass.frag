#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 objectNormal;
layout(location = 2) in vec3 terrainNormal;
layout(location = 3) in vec3 grassColor;
layout(location = 4) in vec2 uv;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"

void main()
{
	vec3 normal = normalize(objectNormal);
	if (!gl_FrontFacing) normal *= -1;

	vec3 bladeColor = mix(grassColor.xyz * 0.5, grassColor.xyz, uv.y);

	vec3 diffuse = DiffuseLighting(terrainNormal, vec3(1));
	vec3 combinedColor = diffuse * bladeColor;

	outColor = vec4(combinedColor, 1.0);
}