#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 1, binding = 2) uniform sampler2D treeDiffuseSampler;

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 worldPosition;
layout(location = 3) in vec4 shadowPositions[2];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"

void main()
{
	float shadow = 1.0;
	vec3 normal = normalize(inNormal);
	//if (variables.shadows == 1) shadow = clamp(1.0 - GetShadow(shadowPosition, 1, 0), 0.3, 1.0);
	if (variables.shadows == 1)
	{
		if (variables.shadowCascades == 1)
		{
			shadow = GetCascadedShadow(shadowPositions, 1, 2.0);
			//shadow = GetCascadedShadow(shadowPositions[1], 1, 0, 2.0);
			//if (shadow < 1.0)
			//{
			//	float tempShadow = GetCascadedShadow(shadowPositions[0], 0, 1, 2.0);
			//	if (tempShadow > shadow) shadow = tempShadow;
			//}
		}
		//else shadow = GetShadow(shadowPosition, 1, -1, 0.25);
	}

	vec3 diffuse = DiffuseLighting(normal, shadow, 0.025);
	vec3 texColor = texture(treeDiffuseSampler, inCoord * 0.25).xyz;
	vec3 combinedColor = diffuse * texColor;
	
	
	//combinedColor *= shadow;
	combinedColor = GroundFog(combinedColor, GetDepth(gl_FragCoord.z), worldPosition.y);

	outColor = vec4(combinedColor, 1.0);

	//outColor = vec4(texColor, 1.0);
}