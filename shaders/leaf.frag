#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 localPosition;
layout(location = 2) in vec3 globalNormal;
layout(location = 3) in vec3 localNormal;
layout(location = 4) in vec3 leafColor;
layout(location = 5) in vec3 localCoordinates;
layout(location = 6) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"
#include "heightmap.glsl"
#include "functions.glsl"

const vec3 leafTint = vec3(0.0916, 0.1, 0.0125);
const vec3 translucencyTint = vec3(235, 196, 5) / 255.0;

const vec4 discardDistances = vec4(0.125 * 1.1, 0.1875, 0.375 * 0.9, 0.0625 * 1.75);
//const vec4 discardDistances = vec4(0.125, 0.1875, 0.375, 0.0625);

const Triangle discardTriangle0 = Triangle(vec2(0.0, 0.25) * 1.5, vec2(0.25, -0.125) * 1.5, vec2(-0.25, -0.125) * 1.5);
const Triangle discardTriangle1 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(-discardDistances.x, discardDistances.y) * 1.425, vec2(-discardDistances.z, discardDistances.w) * 1.425);
const Triangle discardTriangle2 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(-discardDistances.x, -discardDistances.y) * 1.425, vec2(-discardDistances.z, -discardDistances.w) * 1.425);
const Triangle discardTriangle3 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(discardDistances.x, discardDistances.y) * 1.425, vec2(discardDistances.z, discardDistances.w) * 1.425);
const Triangle discardTriangle4 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(discardDistances.x, -discardDistances.y) * 1.425, vec2(discardDistances.z, -discardDistances.w) * 1.425);
const Triangle discardTriangle5 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(0.25, 0.25) * 1.425, vec2(-0.25, 0.25) * 1.425);

bool ShouldDiscard(vec2 position)
{
	if (InsideTriangle(discardTriangle1, position)) return (true);
	else if (InsideTriangle(discardTriangle2, position)) return (true);
	else if (InsideTriangle(discardTriangle3, position)) return (true);
	else if (InsideTriangle(discardTriangle4, position)) return (true);
	else return (false);
}

void main()
{
	if (ShouldDiscard(localCoordinates.xz)) discard;

	vec3 leafNormal = localNormal;
	if (!gl_FrontFacing) leafNormal *= -1;
	if (dot(leafNormal, globalNormal) < 0.0) leafNormal *= -1;
	//if (gl_FrontFacing && dot(leafNormal, globalNormal) < 0.0) leafNormal *= -1;
	//else if (!gl_FrontFacing && dot(leafNormal, globalNormal) > 0.0) leafNormal *= -1;
	leafNormal = mix(leafNormal, globalNormal, 0.5);
	//if (!gl_FrontFacing) leafNormal *= -1;
	float depth = GetDepth(gl_FragCoord.z);
	//float shadow = 0.0;
	float terrainShadow = GetTerrainShadow(worldPosition.xz);
	float shadow = terrainShadow;
	ShadowResults shadowResults;
	shadowResults.reduction = 1.0;
	if (terrainShadow < 1.0)
	{
		shadowResults = GetCascadedShadowResults(shadowPositions, depth);
		shadow = clamp(shadow + shadowResults.shadow, 0.0, 1.0);
	}

	float diffuseEdgeBlend = 1.0;
	//if (shadowResults.lod > 2 && shadowResults.edgeBlend <= 0.5 && shadowResults.edgeBlend >= 0.4) diffuseEdgeBlend = 1.0 - (shadowResults.edgeBlend - 0.4) * 10.0;
	if ((shadowResults.lod == -1 || shadowResults.lod > 2) && shadowResults.reduction < 1.0) 
	{
		//outColor = vec4(vec3(shadowResults.reduction), 1.0);
		//return;
		diffuseEdgeBlend = shadowResults.reduction;
	}

	//float shadow = GetCascadedShadow(shadowPositions, depth);
	//leafNormal = normalize(leafNormal);
	//vec3 terrainNormal = SampleNormalDynamic(worldPosition.xz, 1.0);
	//vec3 leafDiffuse = DiffuseLighting(leafNormal, shadow, 0.25, 0.1);
	vec3 leafDiffuse = DiffuseLighting(leafNormal, shadow, 0.025 + 0.075 * diffuseEdgeBlend, 0.025);
	//vec3 leafDiffuse = DiffuseLightingRealistic(leafNormal, worldPosition, shadow, 0.1, 0.1);
	//vec3 leafDiffuse = DiffuseLighting(leafNormal, shadow);
	vec3 endColor = leafDiffuse * leafTint * leafColor.y;
	if (shadow >= 1.0) endColor *= leafColor.x;

	//float translucency = 1.0;
	float dis = depth * variables.ranges.y;

    if (terrainShadow == 0.0 && dis < 500.0)
	{
		vec3 viewDirection = normalize(worldPosition - variables.viewPosition);
		float normDot = clamp(dot(leafNormal, -variables.lightDirection), 0.0, 1.0);
		normDot += (1.0 - normDot) * 0.2;

		float translucency = pow(clamp(dot(viewDirection, variables.lightDirection), 0.0, 1.0), exp2(10 * 0.625 + 1)) * 1.0 * normDot;
		//translucency *= 1.0 - shadow * 0.975;
		translucency *= 1.0 - shadow * 0.99;
		//if (1.0 - shadow < translucency)
		//{
		//	translucency = (translucency * 0.25 + (1.0 - shadow) * 0.75);
		//	//if (shadow == 0.0) translucency *= 2.0;
		//}

		if (dis > 250.0)
		{
			translucency *= 1.0 - ((dis - 250.0) / 250.0);
		}

		endColor += translucencyTint * leafTint * translucency * 16.0;
	}

	//endColor = GroundFog(endColor, depth, worldPosition.y);
	outColor = vec4(endColor, 1.0);
}