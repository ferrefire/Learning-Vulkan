#version 450

#extension GL_ARB_shading_language_include : require

//#define OBJECT_DATA_COUNT 25
//layout(set = 1, binding = 0) uniform ObjectData
//{
//    mat4 model;
//} objectDatas[OBJECT_DATA_COUNT];
//
//layout(push_constant, std430) uniform PushConstants
//{
//    uint chunkIndex;
//} pc;

//layout(set = 1, binding = 2) uniform sampler2D grassSampler;
layout(set = 1, binding = 1) uniform sampler2D grassDiffuseSampler;
//layout(set = 1, binding = 6) uniform sampler2D grassNormalSampler;
//layout(set = 1, binding = 7) uniform sampler2D grassSpecularSampler;
layout(set = 1, binding = 2) uniform sampler2D rockDiffuseSampler;
layout(set = 1, binding = 3) uniform sampler2D dirtDiffuseSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 shadowPositions[2];
//layout(location = 1) in vec4 shadowLod0Position;
//layout(location = 2) in vec4 shadowLod1Position;
//layout(location = 1) in mat3 tbn;
//layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "heightmap.glsl"
#include "lighting.glsl"
#include "functions.glsl"
#include "depth.glsl"

const float[] textureBlendDistances = {0.5, 0.5};

const float[] textureLod0Distances = {25 * 25, 50 * 50};
const float[] textureLod1Distances = {200 * 200, 375 * 375};
const float[] textureLod2Distances = {1000 * 1000, 1000 * 1000};
const float[] textureLod3Distances = {5000 * 5000, 10000 * 10000};

const float[] textureLod0Scales = {0.25, 0.1};
const float[] textureLod1Scales = {0.05, 0.01};
const float[] textureLod2Scales = {0.01, 0.00375};
const float[] textureLod3Scales = {0.002, 0.001};

const vec3[] defaultColors = {vec3(0.0916, 0.0866, 0.0125), vec3(0.3, 0.175, 0.15)};

const int maxNormalLod = 0;
const float maxNormalDistance = 625;
const float normalBlendDistance = 312.5;

const float steepnessBlendDistance = 0.05;
const float rockSteepness = 0.25;

//const float blendDistance = 0.5;

const vec3 defaultGrassColor = vec3(0.0916, 0.0866, 0.0125);
const vec3 defaultRockColor = vec3(0.2, 0.1, 0.1);

vec3 BlendTexture(sampler2D textureSampler, int index, float viewDistance, int maxLod)
{
	vec3 result = vec3(0);
	float lod0Blend = textureLod0Distances[index] * textureBlendDistances[index];
	float lod1Blend = textureLod1Distances[index] * textureBlendDistances[index];
	float lod2Blend = textureLod2Distances[index] * textureBlendDistances[index];
	float lod3Blend = textureLod3Distances[index] * textureBlendDistances[index];
	float maxDistance = textureLod3Distances[index] - lod3Blend;
	float maxBlend = lod3Blend * 2;

	if (maxLod >= 0)
	{
		if (maxLod == 0)
		{
			maxDistance = textureLod0Distances[index] - lod0Blend;
			maxBlend = lod0Blend * 2;
		}
		else if (maxLod == 1) 
		{
			maxDistance = textureLod1Distances[index] - lod1Blend;
			maxBlend = lod1Blend * 2;
		}
		else if (maxLod == 2) 
		{
			maxDistance = textureLod2Distances[index] - lod2Blend;
			maxBlend = lod2Blend * 2;
		}
		else if (maxLod == 3) 
		{
			maxDistance = textureLod3Distances[index] - lod3Blend;
			maxBlend = lod3Blend * 2;
		}
	}

	if (viewDistance >= maxDistance + maxBlend)
	{
		return (defaultColors[index]);
	}

	viewDistance = min(viewDistance, maxDistance + maxBlend);

	if (viewDistance < textureLod0Distances[index] + lod0Blend)
	{
		result = texture(textureSampler, inPosition.xz * textureLod0Scales[index]).rgb;

		if (viewDistance > textureLod0Distances[index] - lod0Blend)
		{
			float blendFactor = viewDistance - (textureLod0Distances[index] - lod0Blend);
			blendFactor /= lod0Blend * 2;

			result *= (1.0 - blendFactor);
			result += texture(textureSampler, inPosition.xz * textureLod1Scales[index]).rgb * (blendFactor);
		}
	}
	else if (viewDistance < textureLod1Distances[index] + lod1Blend)
	{
		result = texture(textureSampler, inPosition.xz * textureLod1Scales[index]).rgb;

		if (viewDistance > textureLod1Distances[index] - lod1Blend)
		{
			float blendFactor = viewDistance - (textureLod1Distances[index] - lod1Blend);
			blendFactor /= lod1Blend * 2;

			result *= (1.0 - blendFactor);
			result += texture(textureSampler, inPosition.xz * textureLod2Scales[index]).rgb * (blendFactor);
		}
	}
	else if (viewDistance < textureLod2Distances[index] + lod2Blend)
	{
		result = texture(textureSampler, inPosition.xz * textureLod2Scales[index]).rgb;

		if (viewDistance > textureLod2Distances[index] - lod2Blend)
		{
			float blendFactor = viewDistance - (textureLod2Distances[index] - lod2Blend);
			blendFactor /= lod2Blend * 2;

			result *= (1.0 - blendFactor);
			result += texture(textureSampler, inPosition.xz * textureLod3Scales[index]).rgb * (blendFactor);
		}
	}
	else if (viewDistance < textureLod3Distances[index] + lod3Blend)
	{
		result = texture(textureSampler, inPosition.xz * textureLod3Scales[index]).rgb;

		if (viewDistance > textureLod3Distances[index] - lod3Blend)
		{
			float blendFactor = viewDistance - (textureLod3Distances[index] - lod3Blend);
			blendFactor /= lod3Blend * 2;

			result *= (1.0 - blendFactor);
			result += defaultColors[index] * (blendFactor);
		}
	}

	if (viewDistance > maxDistance)
	{
		float blendFactor = viewDistance - (maxDistance);
		blendFactor /= maxBlend;
		result *= (1.0 - blendFactor);
		result += defaultColors[index] * blendFactor;
	}

	return (result);
}

vec3 BlendSteepness(float steepness, float distanceSqrd)
{
	vec3 result;

	if (steepness >= rockSteepness + steepnessBlendDistance)
	{
		result = BlendTexture(rockDiffuseSampler, 1, distanceSqrd, -1);
		return (result);
	}
	if (steepness < rockSteepness + steepnessBlendDistance)
	{
		result = BlendTexture(grassDiffuseSampler, 0, distanceSqrd, -1);
	}
	if (steepness > rockSteepness - steepnessBlendDistance)
	{
		float blendFactor = steepness - (rockSteepness - steepnessBlendDistance);
		blendFactor /= steepnessBlendDistance * 2;

		result *= (1.0 - blendFactor);
		result += BlendTexture(rockDiffuseSampler, 1, distanceSqrd, -1) * blendFactor;
	}

	return (result);
}

void main()
{
	float distanceSqrd = SquaredDistance(inPosition, variables.viewPosition);
	float depth = GetDepth(gl_FragCoord.z);
	//vec3 viewDirection = normalize(variables.viewPosition - inPosition);
	vec3 terrainNormal = SampleNormalDynamic(inPosition.xz, 1.0);
	float steepness = 1.0 - pow(1.0 - GetSteepness(terrainNormal), 1.5);
	//vec3 textureColor = BlendSteepness(steepness, distanceSqrd) * 1.5;
	vec3 textureColor = BlendSteepness(steepness, distanceSqrd);

	float shadow = 0.0;
	if (variables.shadows == 1)
	{
		if (variables.shadowCascades == 1)
		{
			shadow = GetCascadedShadow(shadowPositions, 1, 2.0);
			//shadow = GetCascadedShadow(shadowPositions[1], 1, 0, 2.0);
			//if (shadow < 1.0)
			//{
			//	float tempShadow = GetCascadedShadow(shadowPositions[0], 0, 1, 2.0);
			//	if (tempShadow > shadow) shadow = tempShadow;
			//}
		}
		//else
		//{
		//	shadow = GetShadow(shadowLod1Position, 1, -2);
		//	if (shadow < 1.0)
		//	{
		//		float tempShadow = GetShadow(shadowLod0Position, 0, -1);
		//		if (tempShadow > shadow) shadow = tempShadow;
		//	}
		//}
	}

	vec3 diffuse = DiffuseLighting(terrainNormal, shadow);
	
	//vec4 shadowSpace = variables.shadowProjection * variables.shadowView * vec4(inPosition, 1.0);

	vec3 combinedColor = textureColor * diffuse;
	//vec3 endColor = Fog(combinedColor, depth);
	vec3 endColor = GroundFog(combinedColor, depth, inPosition.y);

	outColor = vec4(endColor, 1.0);
	//outColor = vec4(vec3(steepness), 1.0);
	//outColor = vec4((terrainNormal + 1.0) * 0.5, 1.0);
}