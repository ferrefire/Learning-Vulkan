#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "transformation.glsl"

float center = 0.1;
float outer = 1.0 - center;

float mid = 0.75;
//float inside = 1.0 - outside;

void main()
{
	vec3 finalColor = SKY_COLOR;

	//vec2 lightScreenPosition = variables.lightDirection.xy * 0.5 + 0.5;
	vec2 lightScreenPosition = WorldToClip(variables.viewPosition + variables.lightDirection).xy;
	//vec2 lightScreenPosition = (variables.view * vec4(variables.viewPosition + variables.lightDirection, 1.0)).xy;
	vec2 skyPosition = inCoordinates;
	//skyPosition.y *= 0.5625;

	float intensity = pow(1.0 - clamp(length((lightScreenPosition - skyPosition) * vec2(1.0, 0.5625)), 0.0, 1.0), 4.0);

	if (intensity > 0.001)
	{
		float blendIntensity = clamp(intensity, 0.0, mid) / mid;
		finalColor = mix(vec3(1.0, 0.8, 0.0), SKY_COLOR, pow(1.0 - blendIntensity, 1.0));
		if (intensity > center)
		{
			intensity = (intensity - center) / outer;
			intensity = 1.0 - pow(1.0 - intensity, 1.0);
			finalColor = mix(finalColor, vec3(1), intensity);
		}
	}

    outColor = vec4(finalColor, 1.0);
}