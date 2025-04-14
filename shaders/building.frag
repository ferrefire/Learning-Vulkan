#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(set = 1, binding = 1) uniform sampler2D beamSamplers[3];
layout(set = 1, binding = 2) uniform sampler2D plasteredSamplers[3];
layout(set = 1, binding = 3) uniform sampler2D reedSamplers[3];
layout(set = 1, binding = 4) uniform sampler2D brickSamplers[3];

layout(location = 0) in vec3 objectPosition;
layout(location = 1) in vec3 worldPosition;
layout(location = 2) in vec2 inCoordinate;
layout(location = 3) in vec3 objectNormal;
layout(location = 4) in vec3 worldNormal;
layout(location = 5) in mat4 orientation;
layout(location = 9) in flat ivec2 type;
layout(location = 10) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "sampling.glsl"
#include "depth.glsl"
#include "transformation.glsl"

void main()
{
	//if (type.x == 2) discard;

    vec3 oNormal = normalize(objectNormal);
    vec3 wNormal = normalize(worldNormal);

    vec3 weights = GetWeights(oNormal, 1);

	vec3 worldUV = objectPosition * 0.125;
	vec2 uv = inCoordinate;

	float depth = GetDepth(gl_FragCoord.z);

    vec3 texColor;
	vec3 texNormal;
	vec3 texAmbient;

	if (type.x == 0)
	{
		worldUV.y += 0.065;

		texColor = SampleTriplanarColor(beamSamplers[0], worldUV, weights);
		texNormal = SampleTriplanarNormal(beamSamplers[1], worldUV, weights, oNormal, 1.0);
		texAmbient = SampleTriplanarColor(beamSamplers[2], worldUV, weights);
	}
	else if (type.x == 1)
	{
		uv *= 0.5;
		texColor = SampleTriplanarColor(plasteredSamplers[0], worldUV, weights);
		texNormal = SampleTriplanarNormal(plasteredSamplers[1], worldUV, weights, oNormal, 1.0);
		texAmbient = SampleTriplanarColor(plasteredSamplers[2], worldUV, weights);
	}
	else if (type.x == 2)
	{
		texColor = SampleTriplanarColorFlat(reedSamplers[0], uv, weights);
		texNormal = SampleTriplanarNormalFlat(reedSamplers[1], uv, weights, oNormal, 1.0);
		texAmbient = SampleTriplanarColorFlat(reedSamplers[2], uv, weights);
	}
	else if (type.x == 3)
	{
		texColor = SampleTriplanarColor(brickSamplers[0], worldUV, weights);
		texNormal = SampleTriplanarNormal(brickSamplers[1], worldUV, weights, oNormal, 1.0);
		texAmbient = SampleTriplanarColor(brickSamplers[2], worldUV, weights);
	}
	else if (type.x == 4)
	{
		bool rotate = weights.y < weights.x || weights.y < weights.z;

		worldUV.y += 0.065;

		texColor = SampleTriplanarColor(beamSamplers[0], worldUV, weights, rotate);
		texNormal = SampleTriplanarNormal(beamSamplers[1], worldUV, weights, oNormal, 0.5, rotate);
		texAmbient = SampleTriplanarColor(beamSamplers[2], worldUV, weights, rotate);
	}
	/*else if (type.x == 3)
	{
		uv.xz += 0.5;
		if (type.y == 0)
		{
			uv = uv.yxz;
			weights = weights.yxz;
		}
		if (type.y == 1)
		{
			uv = uv.xzy;
			weights = weights.xzy;
		}
		texColor = SampleTriplanarColor(beamSamplers[0], uv, weights);
		texNormal = SampleTriplanarNormal(beamSamplers[1], uv, weights, normal, 1.0);
		texAmbient = SampleTriplanarColor(beamSamplers[2], uv, weights);
	}
	else if (type.x == 4)
	{
		uv.xz += 0.5;
		texColor = SampleTriplanarColor(beamSamplers[0], uv, weights);
		texNormal = SampleTriplanarNormal(beamSamplers[1], uv, weights, normal, 1.0);
		texAmbient = SampleTriplanarColor(beamSamplers[2], uv, weights);
	}*/

	float shadow = GetTerrainShadow(worldPosition.xz);
	if (shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);

	texNormal = (orientation * vec4(texNormal, 0.0)).xyz;

	//float shadow = GetCascadedShadow(shadowPositions, depth);

    vec3 diffuse = DiffuseLighting(texNormal, shadow, 0.0375);

    vec3 finalLighting = texColor * texAmbient * diffuse;

    outColor = vec4(finalLighting, 1.0);
}