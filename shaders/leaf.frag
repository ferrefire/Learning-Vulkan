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
#include "heightmap.glsl"

void main()
{
	vec3 leafNormal = normal;
	//if (!gl_FrontFacing) leafNormal *= -1;
	float depth = GetDepth(gl_FragCoord.z);
	//float shadow = 0.0;
	float terrainShadow = GetTerrainShadow(worldPosition.xz);
	float shadow = terrainShadow;
	if (terrainShadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);
	//float shadow = GetCascadedShadow(shadowPositions, depth);
	//leafNormal = normalize(leafNormal);
	//vec3 terrainNormal = SampleNormalDynamic(worldPosition.xz, 1.0);
	//vec3 leafDiffuse = DiffuseLighting(leafNormal, shadow, 0.25, 0.1);
	vec3 leafDiffuse = DiffuseLighting(leafNormal, shadow);
	//vec3 leafDiffuse = DiffuseLightingRealistic(leafNormal, worldPosition, shadow, 0.1, 0.1);
	//vec3 leafDiffuse = DiffuseLighting(leafNormal, shadow);
	vec3 endColor = leafDiffuse * leafColor;

	//float translucency = 1.0;
	float dis = depth * variables.ranges.y;

    if (terrainShadow == 0.0 && dis < 500.0)
	{
		vec3 viewDirection = normalize(worldPosition - variables.viewPosition);
		float normDot = clamp(dot(leafNormal, -variables.lightDirection), 0.0, 1.0);
		normDot += (1.0 - normDot) * 0.2;

		float translucency = pow(clamp(dot(viewDirection, variables.lightDirection), 0.0, 1.0), exp2(10 * 0.5 + 1)) * 1.0 * normDot;
		if (1.0 - shadow < translucency)
		{
			translucency = (translucency * 0.25 + (1.0 - shadow) * 0.75);
			//if (shadow == 0.0) translucency *= 2.0;
		}

		if (dis > 250.0)
		{
			translucency *= 1.0 - ((dis - 250.0) / 250.0);
		}

		endColor += lightColor * leafColor * translucency;
	}

	//endColor = GroundFog(endColor, depth, worldPosition.y);
	outColor = vec4(endColor, 1.0);
}