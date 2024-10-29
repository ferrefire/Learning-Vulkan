#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 0, binding = 0) uniform Variables 
{
    vec3 viewPosition;
    vec3 viewDirection;
	vec3 viewRight;
    vec3 viewUp;
	vec4 resolution;
} variables;

layout(set = 1, binding = 0) uniform ObjectData 
{
    mat4 model;
    mat4 view;
    mat4 projection;
} objectData;

//layout(set = 1, binding = 2) uniform sampler2D grassSampler;
layout(set = 1, binding = 5) uniform sampler2D grassDiffuseSampler;
layout(set = 1, binding = 6) uniform sampler2D grassNormalSampler;
layout(set = 1, binding = 7) uniform sampler2D grassSpecularSampler;

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in mat3 tbn;
//layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

#include "heightmap.glsl"
#include "lighting.glsl"
#include "functions.glsl"
#include "depth.glsl"

const float textureLod0Distance = 625; //25
const float textureLod1Distance = 40000; //200
const float textureLod2Distance = 1000000; //1000
const float textureLod3Distance = 25000000; //5000

const float textureLod0Scale = 0.25;
const float textureLod1Scale = 0.05;
const float textureLod2Scale = 0.01;
const float textureLod3Scale = 0.002;

//const float blendDistance = 0.5;

const vec3 defaultColor = vec3(0.0916, 0.0866, 0.0125);

vec3 blendTexture(sampler2D textureSampler, float viewDistance, float maxDistance, float blendDistance, vec3 defaultValue)
{
	vec3 result = vec3(0);
	float lod0Blend = textureLod0Distance * blendDistance;
	float lod1Blend = textureLod1Distance * blendDistance;
	float lod2Blend = textureLod2Distance * blendDistance;
	float lod3Blend = textureLod3Distance * blendDistance;
	if (maxDistance <= 0) maxDistance = textureLod3Distance;
	float lodMaxBlend = maxDistance * blendDistance;

	if (viewDistance > maxDistance + lodMaxBlend)
	{
		return (defaultValue);
	}
	viewDistance = min(viewDistance, maxDistance + lodMaxBlend);
	
	if (viewDistance <= textureLod0Distance - lod0Blend)
	{
		result = texture(textureSampler, inPosition.xz * textureLod0Scale).rgb;
	}
	else if (viewDistance <= textureLod0Distance + lod0Blend)
	{
		float blendFactor = viewDistance - (textureLod0Distance - lod0Blend);
		blendFactor /= lod0Blend * 2;

		result = texture(textureSampler, inPosition.xz * textureLod0Scale).rgb * (1.0 - blendFactor);
		result += texture(textureSampler, inPosition.xz * textureLod1Scale).rgb * (blendFactor);
	}
	else if (viewDistance <= textureLod1Distance - lod1Blend) 
	{
		result = texture(textureSampler, inPosition.xz * textureLod1Scale).rgb;
	}
	else if (viewDistance <= textureLod1Distance + lod1Blend)
	{
		float blendFactor = viewDistance - (textureLod1Distance - lod1Blend);
		blendFactor /= lod1Blend * 2;

		result = texture(textureSampler, inPosition.xz * textureLod1Scale).rgb * (1.0 - blendFactor);
		result += texture(textureSampler, inPosition.xz * textureLod2Scale).rgb * (blendFactor);
	}
	else if (viewDistance <= textureLod2Distance - lod2Blend) 
	{
		result = texture(textureSampler, inPosition.xz * textureLod2Scale).rgb;
	}
	else if (viewDistance <= textureLod2Distance + lod2Blend) 
	{
		float blendFactor = viewDistance - (textureLod2Distance - lod2Blend);
		blendFactor /= lod2Blend * 2;

		result = texture(textureSampler, inPosition.xz * textureLod2Scale).rgb * (1.0 - blendFactor);
		result += texture(textureSampler, inPosition.xz * textureLod3Scale).rgb * (blendFactor);
	}
	else if (viewDistance <= textureLod3Distance + lod3Blend) 
	{
		result = texture(textureSampler, inPosition.xz * textureLod3Scale).rgb;
	}

	if (viewDistance > maxDistance - lodMaxBlend)
	{
		float blendFactor = viewDistance - (maxDistance - lodMaxBlend);
		blendFactor /= lodMaxBlend * 2;

		result *= (1.0 - blendFactor);
		result += defaultValue * blendFactor;
	}

	return (result);
}

void main()
{
	float distanceSqrd = SquaredDistance(inPosition, variables.viewPosition);
	float depth = GetDepth(gl_FragCoord.z);
	vec3 viewDirection = normalize(variables.viewPosition - inPosition);
	vec3 textureColor = blendTexture(grassDiffuseSampler, distanceSqrd, 0, 0.5, defaultColor) * 1.5;
	vec3 terrainNormal = SampleNormalDynamic(inPosition.xz, 1.0);
	vec3 combinedNormal = terrainNormal;

	//vec3 textureNormal = blendTexture(grassNormalSampler, distanceSqrd, 0, 0.5, vec3(1));
	//textureNormal.xy *= 2.0 - 1.0;
	//textureNormal.xy *= 0.25;
	//vec3 tangent = NormalToTangent(terrainNormal);
	//mat3 tangentToWorld = mat3(tangent, cross(terrainNormal, tangent) * -1, terrainNormal);
	//combinedNormal = normalize(tangentToWorld * textureNormal);

	//vec3 specular = SpecularLighting(normalize(specNorm), viewDirection, 100) * blendTexture(grassSpecularSampler, distanceSqrd, 0, 0.5, vec3(0)).r;
	vec3 diffuse = DiffuseLighting(combinedNormal, vec3(1));
	
	vec3 combinedColor = textureColor * diffuse;
	vec3 endColor = Fog(combinedColor, depth);

	outColor = vec4(endColor, 1.0);
}