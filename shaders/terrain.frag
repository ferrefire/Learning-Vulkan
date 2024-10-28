#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 0, binding = 0) uniform Variables 
{
    vec3 viewPosition;
	vec4 resolution;
} variables;

layout(set = 1, binding = 1) uniform sampler2D heightMapSampler;
layout(set = 1, binding = 2) uniform sampler2D grassSampler;

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

#include "heightmap.glsl"
#include "lighting.glsl"
#include "functions.glsl"

void main()
{
	vec3 color = vec3(0.55, 0.52, 0.075) * 0.25;
	float distanceSqrd = SquaredDistance(inPosition, variables.viewPosition);
	if (distanceSqrd < 1000000) color = texture(grassSampler, inPosition.xz * 0.25).rgb * 1.5;
	vec3 normal = SampleNormalDynamic(inPosition.xz, 1.0);
	vec3 diffuse = DiffuseLighting(normal, color);
	vec3 endColor = diffuse;

	outColor = vec4(endColor, 1.0);
}