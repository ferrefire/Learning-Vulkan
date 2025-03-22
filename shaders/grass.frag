#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 objectNormal;
layout(location = 2) in vec3 terrainNormal;
layout(location = 3) in vec3 grassColor;
layout(location = 4) in vec2 uv;
layout(location = 5) in vec4 shadowPositions[CASCADE_COUNT];
//layout(location = 5) in vec4 shadowLod0Position;
//layout(location = 6) in vec4 shadowLod1Position;
//layout(location = 7) in float ao;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"

void main()
{
	//outColor = vec4(1.0);
	//return;

	vec3 normal = normalize(objectNormal);
	vec3 terrainNormal = normalize(terrainNormal);
	float depth = GetDepth(gl_FragCoord.z);

	float shadow = GetTerrainShadow(worldPosition.xz);
	if (shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);
	//float shadow = GetCascadedShadow(shadowPositions, depth);

	//vec3 bladeDiffuse = DiffuseLighting(normal, shadow, 0.0, ambient);

	if (!gl_FrontFacing) normal *= -1;

	//outColor = vec4(normal * 0.5 + 0.5, 1.0);
	//return;

	//float aoDepth = depth;
	//float ao = clamp(depth * variables.ranges.y * 100.0, 0.0, 25000.0);
	//ao /= 25000.0;
	
	//vec3 bladeDiffuse = DiffuseLighting(normal, shadow, 0.0, ambient);

	vec3 bladeColor = mix(grassColor.xyz * 0.3, grassColor.xyz, uv.y);

	vec3 terrainDiffuse = DiffuseLighting(terrainNormal, shadow);
	//vec3 terrainDiffuse = DiffuseLightingRealistic(terrainNormal, worldPosition, shadow);
	
	//vec3 diffuse = clamp(terrainDiffuse * 0.9 + bladeDiffuse, vec3(0.0), lightColor);
	//vec3 diffuse = clamp(terrainDiffuse + bladeDiffuse, vec3(0.0), lightColor);
	vec3 diffuse = terrainDiffuse;
	//vec3 finalColor = FinalLighting(bladeColor, diffuse);
	vec3 finalColor = bladeColor * diffuse;
	//vec3 combinedColor = (diffuse * bladeColor);
	//vec3 viewDirection = normalize(variables.viewPosition - worldPosition);

	if (shadow < 0.9)
	{
		vec3 viewDirection = normalize(variables.viewPosition - worldPosition);
		//vec3 bladeSpecular = SpecularLighting(normal, viewDirection, 16);
		vec3 bladeSpecular = SpecularLighting(normal, viewDirection, 16);
		vec3 terrainSpecular = SpecularLighting(terrainNormal, viewDirection, 32);
		//vec3 bladeSpecular = SpecularLightingRealistic(normal, viewDirection, 16, worldPosition);
		//vec3 terrainSpecular = SpecularLightingRealistic(terrainNormal, viewDirection, 32, worldPosition);
		//combinedColor += (bladeSpecular * terrainSpecular);
		//combinedColor += (bladeSpecular * terrainSpecular) * (1.0 - shadow);
		finalColor += (bladeSpecular * terrainSpecular) * (1.0 - shadow);
		//finalColor += (bladeSpecular) * (1.0 - shadow);
	}
	//else
	//{
	//	combinedColor *= shadow;
	//}

	//vec3 viewDirection = normalize(worldPosition - variables.viewPosition);
	//float normDot = clamp(dot(normal, -variables.lightDirection), 0.0, 1.0);
	//normDot += (1.0 - normDot) * 0.2;
    //float translucency = pow(clamp(dot(-viewDirection, variables.lightDirection), 0.0, 1.0), exp2(10 * 0.25 + 1)) * 2.0 * normDot;
	//if (1.0 - shadow < translucency)
	//	translucency = (translucency * 0.25 + (1.0 - shadow) * 0.75);
	//combinedColor += lightColor * bladeColor * translucency;

	//combinedColor = GroundFog(combinedColor, depth, 0);

	outColor = vec4(finalColor, 1.0);
}