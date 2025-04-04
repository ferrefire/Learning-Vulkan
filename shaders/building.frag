#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(set = 1, binding = 0) uniform sampler2D beamSamplers[3];
layout(set = 1, binding = 1) uniform sampler2D plasteredSamplers[3];
layout(set = 1, binding = 2) uniform sampler2D reedSamplers[3];

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in flat int type;
layout(location = 3) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "sampling.glsl"
#include "depth.glsl"

void main()
{
    vec3 normal = normalize(inNormal);

    vec3 weights = GetWeights(normal, 1);

	vec3 uv = worldPosition * 0.2 + 0.5;

	float depth = GetDepth(gl_FragCoord.z);

    vec3 texColor;
	vec3 texNormal;
	vec3 texAmbient;

	if (type == 0)
	{
		texColor = SampleTriplanarColor(beamSamplers[0], uv, weights);
		texNormal = SampleTriplanarNormal(beamSamplers[1], uv, weights, normal, 1.0);
		texAmbient = SampleTriplanarColor(beamSamplers[2], uv, weights);
	}
	else if (type == 1)
	{
		uv *= 0.5;
		texColor = SampleTriplanarColor(plasteredSamplers[0], uv, weights);
		texNormal = SampleTriplanarNormal(plasteredSamplers[1], uv, weights, normal, 1.0);
		texAmbient = SampleTriplanarColor(plasteredSamplers[2], uv, weights);
	}
	else if (type == 2)
	{
		texColor = SampleTriplanarColor(reedSamplers[0], uv, weights);
		texNormal = SampleTriplanarNormal(reedSamplers[1], uv, weights, normal, 1.0);
		texAmbient = SampleTriplanarColor(reedSamplers[2], uv, weights);
	}

	float shadow = GetTerrainShadow(worldPosition.xz);
	if (shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);

    vec3 diffuse = DiffuseLighting(texNormal, shadow);

    vec3 finalLighting = texColor * texAmbient * diffuse;

    outColor = vec4(finalLighting, 1.0);
}