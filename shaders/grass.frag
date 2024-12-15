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
	vec3 normal = normalize(objectNormal);
	vec3 terrainNormal = normalize(terrainNormal);
	float depth = GetDepth(gl_FragCoord.z);
	//vec3 diffuseNormal = NormalPower(terrainNormal, 0.5);
	//float shadow = 1.0;
	//if (variables.shadows == 1) shadow = clamp(1.0 - GetShadow(shadowLod0Position, 0), 0.3, 1.0);

	//outColor = vec4(grassColor, 1.0);
	//return;

	//float shadow = 0.0;
	float shadow = GetCascadedShadow(shadowPositions, depth);

	vec3 bladeDiffuse = DiffuseLighting(normal, shadow, 0.0, ambient);

	if (!gl_FrontFacing) normal *= -1;

	float ao = clamp(depth * 100.0, 0.0, 1.0);
	
	//if (variables.shadowBounding == 0)
	//{
	//	vec3 shadowSpace = shadowLod0Position.xyz / shadowLod0Position.w;
	//	shadowSpace.xy = shadowSpace.xy * 0.5 + 0.5;
	//	ao = pow(1.0 - shadowSpace.y, 4.0);
	//}
	vec3 bladeColor = mix(grassColor.xyz * mix(0.3, 0.5, ao), grassColor.xyz, uv.y);

	vec3 terrainDiffuse = DiffuseLighting(terrainNormal, shadow);
	
	vec3 diffuse = clamp(terrainDiffuse * 0.9 + bladeDiffuse, vec3(0.0), lightColor);
	vec3 combinedColor = (diffuse * bladeColor);

	if (shadow < 0.9)
	{
		vec3 viewDirection = normalize(variables.viewPosition - worldPosition);
		vec3 bladeSpecular = SpecularLighting(normal, viewDirection, 16);
		vec3 terrainSpecular = SpecularLighting(terrainNormal, viewDirection, 32);
		//combinedColor += (bladeSpecular * terrainSpecular);
		combinedColor += (bladeSpecular * terrainSpecular) * (1.0 - shadow);
	}
	//else
	//{
	//	combinedColor *= shadow;
	//}

	outColor = vec4(combinedColor, 1.0);
}