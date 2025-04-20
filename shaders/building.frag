#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(set = 1, binding = 1) uniform BuildingVariables
{
	vec4 roofTint;
	vec4 wallTint;
	vec4 beamTint;
	vec4 brickTint;
	float roofNormal;
	float wallNormal;
	float beamNormal;
	float brickNormal;
	float roofAmbient;
	float wallAmbient;
	float beamAmbient;
	float brickAmbient;
} buildingVariables;
layout(set = 1, binding = 2) uniform sampler2D beamSamplers[3];
layout(set = 1, binding = 3) uniform sampler2D plasteredSamplers[3];
layout(set = 1, binding = 4) uniform sampler2D reedSamplers[3];
layout(set = 1, binding = 5) uniform sampler2D brickSamplers[3];

layout(location = 0) in vec3 objectPosition;
layout(location = 1) in vec3 worldPosition;
layout(location = 2) in vec2 inCoordinate;
layout(location = 3) in vec3 objectNormal;
layout(location = 4) in mat4 orientation;
layout(location = 8) in flat ivec2 type;
layout(location = 9) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "sampling.glsl"
#include "depth.glsl"
#include "transformation.glsl"

vec2 GetSampleStrength(float normalDistance, float ambientDistance, float dis)
{
	float normalDistanceFallof = normalDistance * 0.2;
	float maxNormalDistance = normalDistance + normalDistanceFallof;
	float ambientDistanceFallof = ambientDistance * 0.2;
	float maxAmbientDistance = ambientDistance + ambientDistanceFallof;

	float normalStrength = (dis > maxNormalDistance ? 0.0 : (1.0 - ((clamp(dis, normalDistance, maxNormalDistance) - normalDistance) / normalDistanceFallof)));
	float ambientStrength = (dis > maxAmbientDistance ? 0.0 : (1.0 - ((clamp(dis, ambientDistance, maxAmbientDistance) - ambientDistance) / ambientDistanceFallof)));

	return (vec2(normalStrength, ambientStrength));
}

void main()
{
	//if (type.x == 2) discard;

    vec3 oNormal = normalize(objectNormal);
    //vec3 wNormal = normalize(worldNormal);

    vec3 weights = GetWeights(oNormal, 1);

	vec3 worldUV = objectPosition * 0.125;
	vec2 uv = inCoordinate;

	float depth = GetDepth(gl_FragCoord.z);

    vec3 texColor;
	vec3 texNormal = oNormal;
	vec3 texAmbient = vec3(1);
	vec3 defaultAmbient = vec3(1);

	float dis = depth * variables.ranges.y;
	vec2 sampleStrength = vec2(0.0);

	float shadow = GetTerrainShadow(worldPosition.xz);
	if (shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);

	if (type.x == 0)
	{
		sampleStrength = GetSampleStrength(50.0, 50.0, dis);

		worldUV.y += 0.065;
		texColor = SampleTriplanarColor(beamSamplers[0], worldUV, weights) * buildingVariables.beamTint.rgb;
		if (shadow < 1.0 && sampleStrength.x > 0.0) texNormal = SampleTriplanarNormal(beamSamplers[1], worldUV, weights, oNormal, buildingVariables.beamNormal * sampleStrength.x);
		if (sampleStrength.y > 0.0)
		{
			if (buildingVariables.beamAmbient != 1.0)
			{
				texAmbient = pow(SampleTriplanarColor(beamSamplers[2], worldUV, weights), vec3(buildingVariables.beamAmbient));
			}
			else
			{
				texAmbient = SampleTriplanarColor(beamSamplers[2], worldUV, weights);
			}
		}
	}
	else if (type.x == 1)
	{
		sampleStrength = GetSampleStrength(50.0, 250.0, dis);

		uv *= 0.5;
		texColor = SampleTriplanarColor(plasteredSamplers[0], worldUV, weights) * buildingVariables.wallTint.rgb;
		if (shadow < 1.0 && sampleStrength.x > 0.0) texNormal = SampleTriplanarNormal(plasteredSamplers[1], worldUV, weights, oNormal, buildingVariables.wallNormal * sampleStrength.x);
		if (sampleStrength.y > 0.0)
		{
			if (buildingVariables.wallAmbient != 1.0)
			{
				texAmbient = pow(SampleTriplanarColor(plasteredSamplers[2], worldUV, weights), vec3(buildingVariables.wallAmbient));
			}
			else
			{
				texAmbient = SampleTriplanarColor(plasteredSamplers[2], worldUV, weights);
			}
		}
	}
	else if (type.x == 2)
	{
		sampleStrength = GetSampleStrength(100.0, 250.0, dis);

		//defaultAmbient = vec3(0.5);
		texColor = SampleTriplanarColorFlat(reedSamplers[0], uv, weights) * buildingVariables.roofTint.rgb;
		if (shadow < 1.0 && sampleStrength.x > 0.0) texNormal = SampleTriplanarNormalFlat(reedSamplers[1], uv, weights, oNormal, buildingVariables.roofNormal * sampleStrength.x);
		if (sampleStrength.y > 0.0)
		{
			if (buildingVariables.roofAmbient != 1.0)
			{
				texAmbient = pow(SampleTriplanarColorFlat(reedSamplers[2], uv, weights), vec3(buildingVariables.roofAmbient));
			}
			else
			{
				texAmbient = SampleTriplanarColorFlat(reedSamplers[2], uv, weights);
			}
		}
	}
	else if (type.x == 3)
	{
		sampleStrength = GetSampleStrength(100.0, 100.0, dis);

		texColor = SampleTriplanarColor(brickSamplers[0], worldUV, weights) * buildingVariables.brickTint.rgb;
		if (shadow < 1.0 && sampleStrength.x > 0.0) texNormal = SampleTriplanarNormal(brickSamplers[1], worldUV, weights, oNormal, buildingVariables.brickNormal * sampleStrength.x);
		if (sampleStrength.y > 0.0)
		{
			if (buildingVariables.brickAmbient != 1.0)
			{
				texAmbient = pow(SampleTriplanarColor(brickSamplers[2], worldUV, weights), vec3(buildingVariables.brickAmbient));
			}
			else
			{
				texAmbient = SampleTriplanarColor(brickSamplers[2], worldUV, weights);
			}
		}
	}
	else if (type.x == 4)
	{
		bool rotate = weights.y < weights.x || weights.y < weights.z;

		sampleStrength = GetSampleStrength(50.0, 50.0, dis);

		worldUV.y += 0.065;
		texColor = SampleTriplanarColor(beamSamplers[0], worldUV, weights, rotate) * buildingVariables.beamTint.rgb;
		if (shadow < 1.0 && sampleStrength.x > 0.0) texNormal = SampleTriplanarNormal(beamSamplers[1], worldUV, weights, oNormal, 0.5 * buildingVariables.beamNormal * sampleStrength.x, rotate);
		if (sampleStrength.y > 0.0)
		{
			if (buildingVariables.beamAmbient != 1.0)
			{
				texAmbient = pow(SampleTriplanarColor(beamSamplers[2], worldUV, weights, rotate), vec3(buildingVariables.beamAmbient));
			}
			else
			{
				texAmbient = SampleTriplanarColor(beamSamplers[2], worldUV, weights, rotate);
			}
		}
	}

	texAmbient = mix(defaultAmbient, texAmbient, sampleStrength.y);

	texNormal = (orientation * vec4(texNormal, 0.0)).xyz;

	//float shadow = GetCascadedShadow(shadowPositions, depth);

    vec3 diffuse = DiffuseLighting(texNormal, shadow, 0.0375);
    //vec3 diffuse = DiffuseLighting(texNormal, shadow);

    vec3 finalLighting = texColor * texAmbient * diffuse;

    outColor = vec4(finalLighting, 1.0);
}