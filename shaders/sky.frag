#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "transformation.glsl"

void main()
{
	vec3 finalColor = vec3(1);

	//vec2 lightScreenPosition = variables.lightDirection.xy * 0.5 + 0.5;
	vec2 lightScreenPosition = WorldToClip(variables.viewPosition + variables.lightDirection).xy;
	//vec2 lightScreenPosition = (variables.view * vec4(variables.viewPosition + variables.lightDirection, 1.0)).xy;
	vec2 skyPosition = inCoordinates;
	//skyPosition.y *= 0.5625;

	float intensity = pow(1.0 - length((lightScreenPosition - skyPosition) * vec2(1.0, 0.5625)), 12.0);

	if (intensity > 0.001) finalColor = mix(vec3(1.0, 0.8, 0.0), vec3(1), pow(1.0 - intensity, 12.0));

    outColor = vec4(finalColor, 1.0);
}