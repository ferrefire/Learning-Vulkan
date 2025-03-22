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
layout(set = 1, binding = 1) uniform sampler2D grassSamplers[3];
layout(set = 1, binding = 2) uniform sampler2D rockSamplers[3];
layout(set = 1, binding = 3) uniform sampler2D dirtSamplers[3];
//layout(set = 1, binding = 6) uniform sampler2D grassNormalSampler;
//layout(set = 1, binding = 7) uniform sampler2D grassSpecularSampler;
//layout(set = 1, binding = 2) uniform sampler2D rockDiffuseSampler;
//layout(set = 1, binding = 3) uniform sampler2D dirtDiffuseSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in flat uint chunkLod;
layout(location = 2) in vec4 shadowPositions[CASCADE_COUNT];

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
#include "sampling.glsl"

//struct BlendDefaults
//{
//	vec3 defaultColor;
//	vec3 defaultNormal;
//	vec3 defaultAO;
//}

#define DIFFUSE 0
#define NORMAL 1
#define AO 2

struct BlendConfig
{
	float blendDistance;
	float startDistance;
	float distanceIncrease;
	float startScale;
	float scaleIncrease;
	float startNormal;
	float normalIncrease;
	float startAmbient;
	float ambientIncrease;
	vec3 defaultColor;
	vec3 tintColor;
	int decreaseLod;
	float decreaseAmount;
};

const float[] textureBlendDistances = {0.5, 0.5};

const float[] textureLod0Distances = {25 * 25, 50 * 50};
const float[] textureLod1Distances = {200 * 200, 375 * 375};
const float[] textureLod2Distances = {1000 * 1000, 1000 * 1000};
const float[] textureLod3Distances = {5000 * 5000, 15000 * 15000};

const float[] textureLod0Scales = {0.25, 0.1};
const float[] textureLod1Scales = {0.05, 0.01};
const float[] textureLod2Scales = {0.01, 0.00375};
const float[] textureLod3Scales = {0.002, 0.002};

const vec3[] defaultColors = {vec3(0.0916, 0.0866, 0.0125), vec3(0.3, 0.175, 0.15)};
const vec3 defaultNormal = vec3(0.5, 0.5, 1.0);
const vec3 defaultAO = vec3(1.0, 1.0, 1.0);

const BlendConfig grassConfig = BlendConfig(0.5, POW(25.0), 30.0, 0.15, 0.2, 0.5, 1.0, 1.5, 1.1, DECODE_COLOR(vec3(93, 99, 44) / 255.0), vec3(1.0), 4, 2.0);
const BlendConfig rockConfig = BlendConfig(0.75, POW(50.0), 25.0, 0.1, 0.2, 1.0, 1.0, 1.0, 1.25, DECODE_COLOR(vec3(145, 141, 128) / 255.0), vec3(1.0), 4, 7.5);
const BlendConfig dirtConfig = BlendConfig(0.75, POW(20.0), 30.0, 0.275, 0.2875, 1.0, 1.0, 1.5, 1.1, DECODE_COLOR(vec3(89, 74, 62) / 255.0), vec3(1.0), 4, 1.0);

//145, 141, 128
//0.0916, 0.0866, 0.0125
//148, 120, 83
//vec3(204, 223, 255)
//vec3(255, 203, 31) * 1.25
//vec3(66, 89, 41)
//vec3(0.0916, 0.0866, 0.0125)
//vec3(185, 201, 167)
//vec3(128, 111, 77)
//vec3(0.3, 0.175, 0.15)
//vec3(209, 155, 107)
//const BlendConfig grassConfig = BlendConfig(0.5, POW(25.0), 30.0, 0.25, 0.2, 1.5, 1.1, pow(vec3(0.0916, 0.0866, 0.0125), vec3(1.0 / 2.2)), 4, 2.0);
//const BlendConfig rockConfig = BlendConfig(0.5, POW(50.0), 25.0, 0.1, 0.2, 1.0, 1.25, pow(vec3(0.3, 0.175, 0.15), vec3(1.0 / 2.2)), 4, 7.5);

const int maxNormalLod = 0;
const float maxNormalDistance = 625;
const float normalBlendDistance = 312.5;

const float steepnessBlendDistance = 0.05;
const float rockSteepness = 0.25;

//const float dirtBlendDistance = 75.0;
//const float dirtHeight = 750.0;

//const float blendDistance = 0.5;

//const vec3 defaultGrassColor = vec3(0.0916, 0.0866, 0.0125);
//const vec3 defaultRockColor = vec3(0.2, 0.1, 0.1);

const float triplanarBlendStrength = 2.0;
const float[] ambientStrengths = {1.5, 1.0};
const int maxLods = 5;

struct BlendResults
{
	vec3 diffuse;
	vec3 normal;
	vec3 ambient;
};

/*vec3 TangentToNormal(vec3 TN, vec3 RN)
{
	vec3 result;

	//vec3 up = TN.z > TN.x ? vec3(1, 0, 0) : vec3(0, 0, 1);
	vec3 up = vec3(0.5, 0.5, 1.0);
	//vec3 terrainTangent;
	//vec3 tan1 = cross(TN, vec3(0.0, 1.0, 0.0));
	//vec3 tan2 = cross(TN, vec3(0.0, 0.0, 1.0));
	//if (length(tan1) > length(tan2)) terrainTangent = tan1;
	//else terrainTangent = tan2;
	vec3 terrainTangent = normalize(cross(TN, up));

	//vec3 up = vec3(1, 0, 0);
    //vec3 terrainTangent = normalize(cross(up, TN));
	//vec3 terrainTangent = normalize(Rotate(TN, radians(90.0), vec3(1.0, 0.0, 0.0)));
	vec3 terrainBiTangent = normalize(cross(TN, terrainTangent));
	//mat3 TBN = mat3(terrainTangent.x, terrainBiTangent.x, TN.x,
	//				terrainTangent.y, terrainBiTangent.y, TN.y,
	//				terrainTangent.z, terrainBiTangent.z, TN.z);
	mat3 TBN = mat3(terrainTangent, terrainBiTangent, TN);
	//result = textureNormal * 2.0 - 1.0;
	result = normalize(TBN * RN);

	return (result);
}*/

vec3 TriplanarBlending(sampler2D textureSampler, vec3 uv, vec3 weights, vec3 normal, int type, float power)
{
	if (type == NORMAL) return (SampleTriplanarNormal(textureSampler, uv, weights, normal, power));
	else return (SampleTriplanarColor(textureSampler, uv, weights));
}

vec3 BlendTexture(sampler2D textureSampler, BlendConfig config, int maxLod, int type, float viewDistance, vec3 weights, vec3 normal)
{
	vec3 result = vec3(0);

	float currentAmbient = config.startAmbient;
	float currentNormal = config.startNormal;

	float currentScale = config.startScale;
	float[maxLods] lodScales;
	for (int i = 0; i < maxLods; i++)
	{
		if (i == config.decreaseLod) currentScale *= config.decreaseAmount;
		lodScales[i] = currentScale;
		currentScale *= config.scaleIncrease;
	}

	float currentDistance = config.startDistance;
	float[maxLods] lodDistances;
	for (int i = 0; i < maxLods; i++)
	{
		lodDistances[i] = currentDistance;
		currentDistance *= config.distanceIncrease;
	}

	float[maxLods] lodBlends;
	for (int i = 0; i < maxLods; i++)
	{
		lodBlends[i] = lodDistances[i] * config.blendDistance;
	}

	float maxDistance = lodDistances[maxLod] - lodBlends[maxLod];
	float maxBlend = lodBlends[maxLod] * 2;

	vec3 texUV = inPosition + variables.terrainOffset;
	//vec3 texUV = inPosition;
	//texUV.xz += variables.terrainOffset.xz;
	//texUV.y -= variables.terrainOffset.y;

	vec3 defaultResult = vec3(0.0);
	if (type == DIFFUSE) defaultResult = config.defaultColor;
	else if (type == NORMAL) defaultResult = normal;
	else if (type == AO) defaultResult = defaultAO;

	if (viewDistance >= maxDistance + maxBlend)
	{
		//return (defaultResult);
		result = defaultResult;
	}
	else
	{
		viewDistance = min(viewDistance, maxDistance + maxBlend);

		for (int i = 0; i < maxLod + 1; i++)
		{
			if (viewDistance < lodDistances[i] + lodBlends[i])
			{
				result = TriplanarBlending(textureSampler, texUV * lodScales[i], weights, normal, type, currentNormal);
				if (type == AO) result = pow(result, vec3(currentAmbient));

				if (viewDistance > lodDistances[i] - lodBlends[i])
				{
					float blendFactor = viewDistance - (lodDistances[i] - lodBlends[i]);
					blendFactor /= lodBlends[i] * 2;

					result *= (1.0 - blendFactor);
					vec3 resultBlend = defaultResult;
					if (i < maxLod) resultBlend = TriplanarBlending(textureSampler, texUV * lodScales[i + 1], weights, normal, type, currentNormal * config.normalIncrease);

					if (type == AO) resultBlend = pow(resultBlend, vec3(currentAmbient * config.ambientIncrease));
					result += resultBlend * (blendFactor);
				}

				break;
			}
			currentAmbient *= config.ambientIncrease;
			currentNormal *= config.normalIncrease;
		}

		if (viewDistance > maxDistance)
		{
			float blendFactor = viewDistance - (maxDistance);
			blendFactor /= maxBlend;
			result *= (1.0 - blendFactor);
			result += defaultResult * blendFactor;
		}
	}

	//if (type == DIFFUSE && config.tintColor != vec3(1.0)) result = DECODE_COLOR(ENCODE_COLOR(result) * config.tintColor);

	return (result);
}

//vec3 TriplanarBlending()

BlendResults BlendSteepness(float height, float steepness, float distanceSqrd, vec3 normal)
{
	BlendResults result;
	result.normal = normal;
	result.ambient = defaultAO;

	vec3 weights = GetWeights(normal, triplanarBlendStrength);

	if (steepness >= rockSteepness + steepnessBlendDistance)
	{
		result.diffuse = BlendTexture(rockSamplers[0], rockConfig, 3, DIFFUSE, distanceSqrd, weights, normal);
		result.normal = BlendTexture(rockSamplers[1], rockConfig, 4, NORMAL, distanceSqrd, weights, normal);
		result.ambient = BlendTexture(rockSamplers[2], rockConfig, 4, AO, distanceSqrd, weights, normal);
		return (result);
	}
	if (height <= variables.waterHeight.x - variables.waterHeight.y)
	{
		result.diffuse = BlendTexture(dirtSamplers[0], dirtConfig, 3, DIFFUSE, distanceSqrd, weights, normal);
		result.normal = BlendTexture(dirtSamplers[1], dirtConfig, 3, NORMAL, distanceSqrd, weights, normal);
		result.ambient = BlendTexture(dirtSamplers[2], dirtConfig, 4, AO, distanceSqrd, weights, normal);
		//return (result);
	}
	else if (steepness < rockSteepness + steepnessBlendDistance)
	{
		result.diffuse = BlendTexture(grassSamplers[0], grassConfig, 3, DIFFUSE, distanceSqrd, weights, normal);
		result.normal = BlendTexture(grassSamplers[1], grassConfig, 4, NORMAL, distanceSqrd, weights, normal);
		result.ambient = BlendTexture(grassSamplers[2], grassConfig, 3, AO, distanceSqrd, weights, normal);
	}
	if (height < variables.waterHeight.x + variables.waterHeight.y)
	{
		float blendFactor = height - (variables.waterHeight.x - variables.waterHeight.y);
		blendFactor /= variables.waterHeight.y * 2;

		result.diffuse *= blendFactor;
		result.normal *= blendFactor;
		result.ambient *= blendFactor;
		result.diffuse += BlendTexture(dirtSamplers[0], dirtConfig, 3, DIFFUSE, distanceSqrd, weights, normal) * (1.0 - blendFactor);
		//result.normal += BlendTexture(dirtSamplers[1], 1, NORMAL, distanceSqrd, weights, normal) * (1.0 - blendFactor);
		//result.normal += normal * (1.0 - blendFactor);
		result.normal += BlendTexture(dirtSamplers[1], dirtConfig, 3, NORMAL, distanceSqrd, weights, normal) * (1.0 - blendFactor);
		vec3 ambient = BlendTexture(dirtSamplers[2], dirtConfig, 4, AO, distanceSqrd, weights, normal);
		result.ambient += ambient * (1.0 - blendFactor);
	}
	if (steepness > rockSteepness - steepnessBlendDistance)
	{
		float blendFactor = steepness - (rockSteepness - steepnessBlendDistance);
		blendFactor /= steepnessBlendDistance * 2;

		result.diffuse *= (1.0 - blendFactor);
		result.normal *= (1.0 - blendFactor);
		result.ambient *= (1.0 - blendFactor);
		result.diffuse += BlendTexture(rockSamplers[0], rockConfig, 3, DIFFUSE, distanceSqrd, weights, normal) * blendFactor;
		//result.normal += BlendTexture(rockSamplers[1], 1, NORMAL, distanceSqrd, weights, normal) * blendFactor;
		//result.normal += defaultNormal * blendFactor;
		result.normal += BlendTexture(rockSamplers[1], rockConfig, 4, NORMAL, distanceSqrd, weights, normal) * blendFactor;
		vec3 ambient = BlendTexture(rockSamplers[2], rockConfig, 4, AO, distanceSqrd, weights, normal);
		result.ambient += ambient * blendFactor;
	}

	return (result);
}

vec3 RotateR(vec3 v, vec3 k, float theta) 
{
    float cosTheta = cos(theta);
    float sinTheta = sin(theta);
    return (v * cosTheta + cross(k, v) * sinTheta + k * dot(k, v) * (1.0 - cosTheta));
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

	//outColor = vec4(vec3(SampleDynamic(inPosition.xz)), 1.0);
	//return;

	float distanceSqrd = SquaredDistance(inPosition, variables.viewPosition);
	float depth = GetDepth(gl_FragCoord.z);
	//vec3 viewDirection = normalize(variables.viewPosition - inPosition);
	//vec3 terrainNormal = SampleNormalDynamic(inPosition.xz, 1.0);

	Normals terrainNormals = SampleNormalsDynamic(inPosition.xz, 1.0);
	vec3 normalWS = terrainNormals.normalWS;
	vec3 normalTS = terrainNormals.normalTS;

	//vec3 texNorm = textureLod(grassSamplers[1], inPosition.xz + variables.terrainOffset.xz, 0.0).rgb;
	//texNorm = TangentToNormal(terrainNormal, texNorm);
	//outColor = vec4(texNorm * 0.5 + 0.5, 1.0);
	//return;

	float steepness = 1.0 - pow(1.0 - GetSteepness(normalWS), 1.5);
	//vec3 textureColor = BlendSteepness(steepness, distanceSqrd) * 1.5;
	//vec3 textureColor = BlendSteepness(steepness, distanceSqrd, terrainNormal);
	BlendResults blendResults = BlendSteepness(inPosition.y + variables.terrainOffset.y, steepness, distanceSqrd, normalWS);
	vec3 textureColor = blendResults.diffuse;
	vec3 textureNormal = blendResults.normal;
	vec3 textureAmbient = DECODE_COLOR(blendResults.ambient);
	
	//textureNormal = vec3(0.5, 0.5, 1.0);
	//textureNormal.xz *= 2.0;
	//vec3 NR = normalize(textureNormal);
	//textureNormal.xy *= 2.0;
	//textureNormal = normalize(textureNormal);

	/*vec3 NR = normalize(textureNormal * 2.0 - 1.0);
	vec3 NT = vec3(0.0, 1.0, 0.0);
    vec3 N0 = vec3(0.0, 0.0, 1.0);
    vec3 axis = normalize(cross(N0, NT));
    float angle = acos(clamp(dot(N0, NT), -1.0, 1.0));
	vec3 RRN = normalize(RotateR(NR, axis, angle));

	//RRN.xz *= 0.5;
	//RRN = normalize(RRN);
	NT = normalWS;
	//NR = textureNormal.xzy * 2.0 - 1.0;
    N0 = vec3(0.0, 1.0, 0.0);
    axis = normalize(cross(N0, NT));
    angle = acos(clamp(dot(N0, NT), -1.0, 1.0));
	vec3 RN = normalize(RotateR(RRN, axis, angle));*/

	vec3 RN = textureNormal;
	//RN = (RN * 2.0 - 1.0);
	//RN = normalize(Rotate(RN, radians(90.0), vec3(1, 0, 0)));

	//vec3 NT = normalWS;
    //vec3 N0 = vec3(0.0, 1.0, 0.0);
    //vec3 axis = normalize(cross(N0, NT));
    //float angle = acos(clamp(dot(N0, NT), -1.0, 1.0));
	//RN = normalize(RotateR(RN, axis, angle));

	//outColor = vec4(0.0, textureNormal.g, 0.0, 1.0);
	//outColor = vec4(textureNormal * 0.5 + 0.5, 1.0);
	//return;

	//vec3 RN = Rotate(NR, radians(90.0), vec3(0.0, 0.0, 1.0));

	//vec3 NT = normalWS;
	//vec3 NR = textureNormal;
	////NR.xy -= 0.5;
	////NR = normalize(NR);
	//vec3 RN = TangentToNormal(NT, NR);

	//vec3 RN;
	//RN.x = NT.y * RRN.x + RRN.y * NT.x;
	//RN.z = NT.y * RRN.z + RRN.y * NT.z;
	//RN.y = NT.y * RRN.y - (NT.x * RRN.x + NT.z * RRN.z);
	//RN = normalize(RN);
	
	//vec3 endNormal = RN;
	//vec3 endNormal = normalize(RN);

	//textureNormal = normalize(textureNormal.xzy * 2.0 - 1.0);
	//vec3 endNormal = normalize(normalWS + RN * 0.25);
	vec3 endNormal = RN;

	//outColor = vec4((depth > 0.1 ? normalWS : endNormal) * 0.5 + 0.5, 1.0);
	//outColor = vec4(normalWS * 0.5 + 0.5, 1.0);
	//return;

	float shadow = 0.0;
	shadow = GetTerrainShadow(inPosition.xz);
	if (shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);

	//float shadow = GetCascadedShadow(shadowPositions, depth);

	vec3 diffuse = DiffuseLighting(endNormal, shadow);
	//vec3 diffuse = DiffuseLighting(normalWS, shadow);
	//vec3 terrainDiffuse = DiffuseLighting(terrainNormal.xzy, shadow);
	//vec3 diffuse = clamp(terrainDiffuse * normalDiffuse, vec3(ambient), terrainDiffuse);
	
	//vec4 shadowSpace = variables.shadowProjection * variables.shadowView * vec4(inPosition, 1.0);

	//vec3 combinedColor = textureColor * diffuse;
	//vec3 combinedColor = textureColor * textureAmbient * diffuse;
	//vec3 endColor = Fog(combinedColor, depth);
	//vec3 endColor = GroundFog(combinedColor, depth, inPosition.y);
	//vec3 endColor = combinedColor;
	//vec3 endColor = ToStandard(combinedColor);

	//if (chunkLod == 1)
	//{
	//	//endColor = vec3(0.0);
	//	//gl_FragDepth = gl_FragCoord.z;
	//}

	//textureNormal = textureNormal * 0.5 + 0.5;
	//if (blendResults.index == 1) textureNormal = terrainNormal * 0.5 + 0.5;
	//if (inLod == 1) gl_FragDepth = 0.999999 + gl_FragCoord.z * 0.000001;
	//if (inLod == 1) gl_FragDepth = 0.99999999;

	//outColor = vec4(textureNormal, 1.0);

	//vec3 finalColor = FinalLighting(textureColor * textureAmbient, diffuse);
	vec3 finalColor = textureColor * textureAmbient * diffuse;

	outColor = vec4(finalColor, 1.0);
}