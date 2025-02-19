#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

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
//layout(set = 1, binding = 1) uniform sampler2D grassDiffuseSampler;
layout(set = 1, binding = 1) uniform sampler2D grassSamplers[2];
layout(set = 1, binding = 2) uniform sampler2D rockSamplers[2];
//layout(set = 1, binding = 6) uniform sampler2D grassNormalSampler;
//layout(set = 1, binding = 7) uniform sampler2D grassSpecularSampler;
//layout(set = 1, binding = 2) uniform sampler2D rockDiffuseSampler;
layout(set = 1, binding = 3) uniform sampler2D dirtDiffuseSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 shadowPositions[CASCADE_COUNT];
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
#include "transformation.glsl"

const float[] textureBlendDistances = {0.5, 0.5};

const float[] textureLod0Distances = {25 * 25, 50 * 50};
const float[] textureLod1Distances = {200 * 200, 375 * 375};
const float[] textureLod2Distances = {1000 * 1000, 1000 * 1000};
const float[] textureLod3Distances = {5000 * 5000, 10000 * 10000};

const float[] textureLod0Scales = {0.25, 0.1};
const float[] textureLod1Scales = {0.05, 0.01};
const float[] textureLod2Scales = {0.01, 0.00375};
const float[] textureLod3Scales = {0.002, 0.002};

const vec3[] defaultColors = {vec3(0.0916, 0.0866, 0.0125), vec3(0.3, 0.175, 0.15)};

const int maxNormalLod = 0;
const float maxNormalDistance = 625;
const float normalBlendDistance = 312.5;

const float steepnessBlendDistance = 0.05;
const float rockSteepness = 0.25;

//const float blendDistance = 0.5;

const vec3 defaultGrassColor = vec3(0.0916, 0.0866, 0.0125);
const vec3 defaultRockColor = vec3(0.2, 0.1, 0.1);

struct BlendResults
{
	vec3 diffuse;
	vec3 normal;
};

vec3 TangentToNormal(vec3 terrainNormal, vec3 textureNormal)
{
	vec3 result;

	//vec3 up = abs(terrainNormal.z) > abs(terrainNormal.x) ? vec3(1, 0, 0) : vec3(0, 0, -1);
	vec3 terrainTangent;
	vec3 tan1 = cross(terrainNormal, vec3(0.0, 0.0, 1.0));
	vec3 tan2 = cross(terrainNormal, vec3(0.0, 1.0, 0.0));
	if (length(tan1) > length(tan2)) terrainTangent = tan1;
	else terrainTangent = tan2;

	//vec3 up = vec3(1, 0, 0);
    //vec3 terrainTangent = normalize(cross(up, terrainNormal));
	//vec3 terrainTangent = normalize(Rotate(terrainNormal, radians(90.0), vec3(1.0, 0.0, 0.0)));
	vec3 terrainBiTangent = cross(terrainTangent, terrainNormal) * 1.0;
	//mat3 TBN = mat3(terrainTangent.x, terrainBiTangent.x, terrainNormal.x,
	//				terrainTangent.y, terrainBiTangent.y, terrainNormal.y,
	//				terrainTangent.z, terrainBiTangent.z, terrainNormal.z);
	mat3 TBN = mat3(terrainTangent, terrainBiTangent, terrainNormal);
	//result = textureNormal * 2.0 - 1.0;
	result = normalize(TBN * textureNormal);

	return (result);
}

vec3 BlendTexture(sampler2D textureSampler, int index, float viewDistance, int maxLod, bool normal)
{
	vec3 result = vec3(0);
	float lod0Blend = textureLod0Distances[index] * textureBlendDistances[index];
	float lod1Blend = textureLod1Distances[index] * textureBlendDistances[index];
	float lod2Blend = textureLod2Distances[index] * textureBlendDistances[index];
	float lod3Blend = textureLod3Distances[index] * textureBlendDistances[index];
	float maxDistance = textureLod3Distances[index] - lod3Blend;
	float maxBlend = lod3Blend * 2;
	vec2 texUV = inPosition.xz + variables.terrainOffset.xz;

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
		return (normal ? vec3(0.0, 0.0, 1.0) : defaultColors[index]);
	}

	viewDistance = min(viewDistance, maxDistance + maxBlend);

	if (viewDistance < textureLod0Distances[index] + lod0Blend)
	{
		result = texture(textureSampler, texUV * textureLod0Scales[index]).rgb;

		if (viewDistance > textureLod0Distances[index] - lod0Blend)
		{
			float blendFactor = viewDistance - (textureLod0Distances[index] - lod0Blend);
			blendFactor /= lod0Blend * 2;

			result *= (1.0 - blendFactor);
			result += texture(textureSampler, texUV * textureLod1Scales[index]).rgb * (blendFactor);
		}
	}
	else if (viewDistance < textureLod1Distances[index] + lod1Blend)
	{
		result = texture(textureSampler, texUV * textureLod1Scales[index]).rgb;

		if (viewDistance > textureLod1Distances[index] - lod1Blend)
		{
			float blendFactor = viewDistance - (textureLod1Distances[index] - lod1Blend);
			blendFactor /= lod1Blend * 2;

			result *= (1.0 - blendFactor);
			result += texture(textureSampler, texUV * textureLod2Scales[index]).rgb * (blendFactor);
		}
	}
	else if (viewDistance < textureLod2Distances[index] + lod2Blend)
	{
		result = texture(textureSampler, texUV * textureLod2Scales[index]).rgb;

		if (viewDistance > textureLod2Distances[index] - lod2Blend)
		{
			float blendFactor = viewDistance - (textureLod2Distances[index] - lod2Blend);
			blendFactor /= lod2Blend * 2;

			result *= (1.0 - blendFactor);
			result += texture(textureSampler, texUV * textureLod3Scales[index]).rgb * (blendFactor);
		}
	}
	else if (viewDistance < textureLod3Distances[index] + lod3Blend)
	{
		result = texture(textureSampler, texUV * textureLod3Scales[index]).rgb;

		if (viewDistance > textureLod3Distances[index] - lod3Blend)
		{
			float blendFactor = viewDistance - (textureLod3Distances[index] - lod3Blend);
			blendFactor /= lod3Blend * 2;

			result *= (1.0 - blendFactor);
			result += (normal ? vec3(0.0, 0.0, 1.0) : defaultColors[index]) * (blendFactor);
		}
	}

	if (viewDistance > maxDistance)
	{
		float blendFactor = viewDistance - (maxDistance);
		blendFactor /= maxBlend;
		result *= (1.0 - blendFactor);
		result += (normal ? vec3(0.0, 0.0, 1.0) : defaultColors[index]) * blendFactor;
	}

	return (result);
}

BlendResults BlendSteepness(float steepness, float distanceSqrd)
{
	BlendResults result;

	if (steepness >= rockSteepness + steepnessBlendDistance)
	{
		result.diffuse = BlendTexture(rockSamplers[0], 1, distanceSqrd, -1, false);
		result.normal = BlendTexture(rockSamplers[1], 1, distanceSqrd, -1, true);
		return (result);
	}
	if (steepness < rockSteepness + steepnessBlendDistance)
	{
		result.diffuse = BlendTexture(grassSamplers[0], 0, distanceSqrd, -1, false);
		result.normal = BlendTexture(grassSamplers[1], 0, distanceSqrd, -1, true);
		//result.normal = vec3(0.0, 0.0, 1.0);
	}
	if (steepness > rockSteepness - steepnessBlendDistance)
	{
		float blendFactor = steepness - (rockSteepness - steepnessBlendDistance);
		blendFactor /= steepnessBlendDistance * 2;

		result.diffuse *= (1.0 - blendFactor);
		result.normal *= (1.0 - blendFactor);
		result.diffuse += BlendTexture(rockSamplers[0], 1, distanceSqrd, -1, false) * blendFactor;
		result.normal += (BlendTexture(rockSamplers[1], 1, distanceSqrd, -1, true)) * blendFactor;
	}

	return (result);
}

vec3 RotateR(vec3 v, vec3 k, float theta) 
{
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    return (v * cosTheta +
           cross(k, v) * sinTheta +
           k * dot(k, v) * (1.0 - cosTheta));
}


void main()
{
	//vec2 terrainShadowUV = (inPosition.xz - variables.terrainShadowOffset) / 5000.0;
	//if (abs(terrainShadowUV.x) < 0.5 && abs(terrainShadowUV.y) < 0.5)
	//{
	//	float terrainShadow = GetTerrainShadow(terrainShadowUV + 0.5);
	//	outColor = vec4(vec3(terrainShadow), 1.0);
	//	return;
	//}

	float distanceSqrd = SquaredDistance(inPosition, variables.viewPosition);
	float depth = GetDepth(gl_FragCoord.z);
	//vec3 viewDirection = normalize(variables.viewPosition - inPosition);
	vec3 terrainNormal = SampleNormalDynamic(inPosition.xz, 1.0);

	//vec3 texNorm = textureLod(grassSamplers[1], inPosition.xz + variables.terrainOffset.xz, 0.0).rgb;
	//texNorm = TangentToNormal(terrainNormal, texNorm);
	//outColor = vec4(texNorm * 0.5 + 0.5, 1.0);
	//return;

	float steepness = 1.0 - pow(1.0 - GetSteepness(terrainNormal), 1.5);
	//vec3 textureColor = BlendSteepness(steepness, distanceSqrd) * 1.5;
	//vec3 textureColor = BlendSteepness(steepness, distanceSqrd, terrainNormal);
	BlendResults blendResults = BlendSteepness(steepness, distanceSqrd);
	vec3 textureColor = blendResults.diffuse;
	vec3 textureNormal = blendResults.normal;
	//textureNormal.xz *= 2.0;
	//vec3 NR = normalize(textureNormal);
	//textureNormal.xy *= 2.0;
	//textureNormal = normalize(textureNormal);

	vec3 NT = terrainNormal;
	vec3 NR = textureNormal.xzy * 2.0 - 1.0;
	//NR.xz *= 0.25;
	NR = normalize(NR);

    vec3 N0 = vec3(0.0, 1.0, 0.0);
    vec3 axis = normalize(cross(N0, NT));
    float angle = acos(clamp(dot(N0, NT), -1.0, 1.0));
	vec3 RN = RotateR(NR, axis, angle);

	//vec3 RN;
	//RN.x = NT.y * NR.x + NR.y * NT.x;
	//RN.z = NT.y * NR.z + NR.y * NT.z;
	//RN.y = NT.y * NR.y - (NT.x * NR.x + NT.z * NR.z);
	//RN = normalize(RN + NT);
	
	vec3 endNormal = normalize(RN + NT);

	//outColor = vec4(endNormal * 0.5 + 0.5, 1.0);
	//return;

	float shadow = 0.0;
	shadow = GetTerrainShadow(inPosition.xz);
	if (shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);

	//float shadow = GetCascadedShadow(shadowPositions, depth);

	vec3 diffuse = DiffuseLighting(endNormal, shadow);
	//vec3 terrainDiffuse = DiffuseLighting(terrainNormal.xzy, shadow);
	//vec3 diffuse = clamp(terrainDiffuse * normalDiffuse, vec3(ambient), terrainDiffuse);
	
	//vec4 shadowSpace = variables.shadowProjection * variables.shadowView * vec4(inPosition, 1.0);

	vec3 combinedColor = textureColor * diffuse;
	//vec3 endColor = Fog(combinedColor, depth);
	//vec3 endColor = GroundFog(combinedColor, depth, inPosition.y);
	vec3 endColor = combinedColor;

	//textureNormal = textureNormal * 0.5 + 0.5;
	//if (blendResults.index == 1) textureNormal = terrainNormal * 0.5 + 0.5;

	//outColor = vec4(textureNormal, 1.0);
	outColor = vec4(endColor, 1.0);
}