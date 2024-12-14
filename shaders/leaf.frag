#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 leafColor;
layout(location = 3) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"

void main()
{
	vec3 leafNormal = normalize(normal);
	if (!gl_FrontFacing) leafNormal *= -1;
	float depth = GetDepth(gl_FragCoord.z);
	float shadow = 0.0;
	if (variables.shadows == 1) shadow = GetCascadedShadow(shadowPositions, depth);
	//leafNormal = normalize(leafNormal);
	vec3 leafDiffuse = DiffuseLighting(leafNormal, shadow, 0.5, 0.1);
	vec3 endColor = leafDiffuse * leafColor;
	endColor = GroundFog(endColor, depth, worldPosition.y);
	outColor = vec4(endColor, 1.0);
}