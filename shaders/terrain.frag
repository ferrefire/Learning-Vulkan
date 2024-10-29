#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 0, binding = 0) uniform Variables 
{
    vec3 viewPosition;
	vec4 resolution;
} variables;

//layout(set = 1, binding = 2) uniform sampler2D grassSampler;
layout(set = 1, binding = 5) uniform sampler2D grassSampler;

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

#include "heightmap.glsl"
#include "lighting.glsl"
#include "functions.glsl"

const float textureLod0Distance = 625; //25
const float textureLod1Distance = 40000; //200
const float textureLod2Distance = 1000000; //1000
const float textureLod3Distance = 25000000; //5000

const float textureLod0Scale = 0.25;
const float textureLod1Scale = 0.05;
const float textureLod2Scale = 0.01;
const float textureLod3Scale = 0.002;

const float blendDistance = 0.5;

vec3 blendTexture(float viewDistance)
{
	vec3 result = vec3(0.55, 0.52, 0.075) * 0.166666;
	float lod0Blend = textureLod0Distance * blendDistance;
	float lod1Blend = textureLod1Distance * blendDistance;
	float lod2Blend = textureLod2Distance * blendDistance;

	if (viewDistance <= textureLod0Distance - lod0Blend)
	{
		result = texture(grassSampler, inPosition.xz * textureLod0Scale).rgb;
	}
	else if (viewDistance <= textureLod0Distance + lod0Blend)
	{
		float blendFactor = viewDistance - (textureLod0Distance - lod0Blend);
		blendFactor /= lod0Blend * 2;

		result = texture(grassSampler, inPosition.xz * textureLod0Scale).rgb * (1.0 - blendFactor);
		result += texture(grassSampler, inPosition.xz * textureLod1Scale).rgb * (blendFactor);
	}
	else if (viewDistance <= textureLod1Distance - lod1Blend) 
	{
		result = texture(grassSampler, inPosition.xz * textureLod1Scale).rgb;
	}
	else if (viewDistance <= textureLod1Distance + lod1Blend)
	{
		float blendFactor = viewDistance - (textureLod1Distance - lod1Blend);
		blendFactor /= lod1Blend * 2;

		result = texture(grassSampler, inPosition.xz * textureLod1Scale).rgb * (1.0 - blendFactor);
		result += texture(grassSampler, inPosition.xz * textureLod2Scale).rgb * (blendFactor);
	}
	else if (viewDistance <= textureLod2Distance - lod2Blend) 
	{
		result = texture(grassSampler, inPosition.xz * textureLod2Scale).rgb;
	}
	else if (viewDistance <= textureLod2Distance + lod2Blend) 
	{
		float blendFactor = viewDistance - (textureLod2Distance - lod2Blend);
		blendFactor /= lod2Blend * 2;

		result = texture(grassSampler, inPosition.xz * textureLod2Scale).rgb * (1.0 - blendFactor);
		result += texture(grassSampler, inPosition.xz * textureLod3Scale).rgb * (blendFactor);
	}
	else if (viewDistance <= textureLod3Distance) 
	{
		result = texture(grassSampler, inPosition.xz * textureLod3Scale).rgb;
	}

	return (result);
}

void main()
{
	float distanceSqrd = SquaredDistance(inPosition, variables.viewPosition);

	vec3 color = blendTexture(distanceSqrd) * 1.5;

	vec3 normal = SampleNormalDynamic(inPosition.xz, 1.0);

	vec3 diffuse = DiffuseLighting(normal, color);
	vec3 endColor = diffuse;

	outColor = vec4(endColor, 1.0);
}