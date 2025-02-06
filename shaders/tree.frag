#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout(set = 1, binding = 2) uniform sampler2D treeDiffuseSampler;

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 worldPosition;
layout(location = 3) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

const vec3 trunkColor = (vec3(156, 121, 70) / 255.0);

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"

void main()
{
	//float shadow = 0.0;
	vec3 normal = normalize(inNormal);
	float depth = GetDepth(gl_FragCoord.z);
	//if (variables.shadows == 1) shadow = clamp(1.0 - GetShadow(shadowPosition, 1, 0), 0.3, 1.0);
	float shadow = GetTerrainShadow(worldPosition.xz);
	if (shadow < 1.0)
		shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);
	//float shadow = GetCascadedShadow(shadowPositions, depth);

	vec3 diffuse = DiffuseLighting(normal, shadow, 0.025);
	vec3 texColor = texture(treeDiffuseSampler, inCoord * 0.25).xyz * trunkColor;
	vec3 combinedColor = diffuse * texColor;
	
	
	//combinedColor *= shadow;
	//combinedColor = GroundFog(combinedColor, depth, worldPosition.y);

	outColor = vec4(combinedColor, 1.0);

	//outColor = vec4(texColor, 1.0);
}