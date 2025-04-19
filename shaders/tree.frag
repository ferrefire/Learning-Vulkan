#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

#ifndef SIDE_COUNT
#define SIDE_COUNT 8
#endif

layout(set = 1, binding = 2) uniform TrunkVariables
{
	vec4 trunkLodColor;
	vec4 trunkTint;
} trunkVariables;
layout(set = 1, binding = 3) uniform sampler2D treeSamplers[3];
//layout(set = 1, binding = 2) uniform sampler2D treeDiffuseSampler;
//layout(set = 1, binding = 3) uniform sampler2D treeLodSamplers[SIDE_COUNT];

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 worldPosition;
layout(location = 3) in flat uint lod;
layout(location = 4) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

//const vec3 trunkColor = (vec3(156, 121, 70) / 255.0);
//const vec3 trunkLodColor = (vec3(48, 32, 16) / 255.0);

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"
#include "sampling.glsl"

void main()
{
	//outColor = vec4(vec3(inCoord.x), 1.0);
	//return;

	//float shadow = 0.0;
	vec3 normal = normalize(inNormal);
	float depth = GetDepth(gl_FragCoord.z);
	//if (variables.shadows == 1) shadow = clamp(1.0 - GetShadow(shadowPosition, 1, 0), 0.3, 1.0);
	float shadow = GetTerrainShadow(worldPosition.xz);
	if (lod < 5 && shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);
	//float shadow = GetCascadedShadow(shadowPositions, depth);

	//vec3 diffuse = DiffuseLightingRealistic(normal, worldPosition, shadow, 0.025, 0.025);
	//vec3 texColor = trunkLodColor;
	vec3 texColor = trunkVariables.trunkLodColor.rgb;
	vec3 texNormal = normal;
	vec3 texAmbient = vec3(1.0);
	float dis = depth * variables.ranges.y;

	if (dis < 250.0)
	{
		vec3 weights = GetWeights(normal, 2.0);
		texColor = SampleTriplanarColor(treeSamplers[0], worldPosition * 0.1, weights);
		if (shadow < 1.0) texNormal = SampleTriplanarNormal(treeSamplers[1], worldPosition * 0.1, weights, normal, 1.0 + (dis / 250.0) * 3.0);
		texAmbient = SampleTriplanarColor(treeSamplers[2], worldPosition * 0.1, weights);

		//texColor = texture(treeSamplers[0], inCoord * 0.25).xyz;
		//texAmbient = (texture(treeSamplers[2], inCoord * 0.25).xyz);
	}
	
	vec3 diffuse = DiffuseLighting(texNormal, shadow, 0.025 * 0.5);
	vec3 combinedColor = diffuse * (texColor * trunkVariables.trunkTint.rgb) * texAmbient;
	
	
	//combinedColor *= shadow;
	//combinedColor = GroundFog(combinedColor, depth, worldPosition.y);

	outColor = vec4(combinedColor, 1.0);

	//outColor = vec4(texColor, 1.0);
}