#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 1, binding = 2) uniform sampler2D treeDiffuseSampler;

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec4 shadowPosition;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"

void main()
{
	vec3 diffuse = DiffuseLighting(inNormal, vec3(1));
	vec3 texColor = texture(treeDiffuseSampler, inCoord * 0.25).xyz;
	vec3 combinedColor = diffuse * texColor;
	float shadow = 1.0;
	if (variables.shadows == 1) shadow = clamp(1.0 - GetShadow(shadowPosition, 1), 0.3, 1.0);
	combinedColor *= shadow;
	combinedColor = Fog(combinedColor, GetDepth(gl_FragCoord.z));

	outColor = vec4(combinedColor, 1.0);

	//outColor = vec4(texColor, 1.0);
}