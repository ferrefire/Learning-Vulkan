#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout (input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;
layout(set = 1, binding = 2) uniform sampler2D waterNormalSampler;

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "functions.glsl"
#include "lighting.glsl"

const vec3 waterTint = DECODE_COLOR(vec3(5, 41, 35) / 255.0);
const float waterDensity = 2500.0;

void main()
{
	float depth = subpassLoad(inputDepth).r;
	if (gl_FragCoord.z > depth) discard;

	//float bottomDepth = clamp(GetDepth(depth) * variables.ranges.y, 0.0, 500.0) / 500.0;
	//float surfaceDepth = clamp(GetDepth(gl_FragCoord.z) * variables.ranges.y, 0.0, 500.0) / 500.0;
	float bottomDepth = GetDepth(depth);
	float surfaceDepth = GetDepth(gl_FragCoord.z);

	float waterDepth = (bottomDepth - surfaceDepth);
	float fogFactor = clamp(exp2(-waterDensity * waterDepth), 0.25, 1.0) - 0.25;

	float shadow = 1.0 - GetTerrainShadow(worldPosition.xz);

	vec3 normal = (texture(waterNormalSampler, worldPosition.xz * 0.0075).rgb * 2.0 - 1.0).xzy;
	normal.xz *= 1.5;
	normal = normalize(normal);

	vec3 viewDirection = normalize(variables.viewPosition - worldPosition);

	vec3 originalColor = subpassLoad(inputColor).rgb;

	//vec3 diffuse = DiffuseLighting(normal, (1.0 - shadow));
	//vec3 surface = mix(waterTint, skyColor.rgb, pow(max(dot(normal, vec3(0.0, 1.0, 0.0)), 0.0), 16.0));
	vec3 surface = skyColor.rgb;
	//vec3 diffuse = surface * mix(0.5, 1.0, shadow);
	vec3 diffuse = surface * DiffuseLighting(normal, (1.0 - shadow), 0.25);
	vec3 specular = SpecularLighting(normal, viewDirection, 128.0 * 4.0);

	//vec3 finalColor = skyColor.rgb * (waterDepth) + originalColor * (1.0 - waterDepth);
	vec3 finalColor = mix(diffuse, originalColor, fogFactor) + (specular * shadow);

	outColor = vec4(finalColor, 1.0);
}