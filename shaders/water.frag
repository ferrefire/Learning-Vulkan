#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

layout (input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;
layout (set = 1, binding = 2) uniform sampler2D viewSampler;
layout (set = 1, binding = 3) uniform sampler3D aerialSampler;
layout(set = 1, binding = 4) uniform sampler2D waterNormalSampler;

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "functions.glsl"
#include "lighting.glsl"
#include "atmosphere.glsl"
#include "transformation.glsl"
#include "heightmap.glsl"

const vec3 waterTint = DECODE_COLOR(vec3(5, 41, 35) / 255.0);
//const float waterDensity = 750.0;
const float waterDensity = 2000.0;
const vec2 scatterResolution = vec2(32, 32);

const int maxBlend = 1;
const float startDistance = 10000.0;
const float distanceIncrease = 5.0;
const float startScale = 0.075;
const float scaleIncrease = 0.2;
const float startSpeed = 0.01;
const float speedIncrease = 1.0;
const float startPower = 1.0;
const float powerIncrease = 1.0;
const float blendDistance = 0.75;
//const float startSkyPower = 4.0;
//const float skyPowerIncrease = 1.5;
//const float startSpecularPower = 2.0;
//const float specularPowerIncrease = 1.25;


struct WaveNormals
{
	vec3 skyNormal;
	vec3 specularNormal;
};

vec3 SampleNormal(vec2 uv, float power)
{
	vec3 normal = (texture(waterNormalSampler, uv).rgb * 2.0 - 1.0).xzy;
	normal.xz *= power;
	normal = normalize(normal);

	return (normal);
}

vec3 GetSkyColor(vec3 normal)
{
	vec3 rayDirection = normal;
	rayDirection = normalize(Rotate(rayDirection, radians(-90.0), vec3(1.0, 0.0, 0.0)));
	vec3 sunDirection = variables.rotatedLightDirection;
	float lightAngle = acos(dot(normalize(vec3(sunDirection.xy, 0.0)), normalize(vec3(rayDirection.xy, 0.0))));
	vec3 rayStart = vec3(0.0, 0.0, PR + RADIUS_OFFSET + (worldPosition.y + variables.terrainOffset.y) * 0.001);
	float viewHeight = length(rayStart);
	vec3 upVector = normalize(rayStart);
	float viewAngle = acos(dot(rayDirection, upVector));
	bool surfaceIntersect = SphereIntersectNearest(rayStart, rayDirection, PR) >= 0.0;
	vec2 uv = ViewUV(vec2(viewAngle, lightAngle), scatterResolution, viewHeight, surfaceIntersect);
	vec3 currentSkyColor = texture(viewSampler, uv).rgb;

	return (currentSkyColor);
}

vec4 GetAerialColor()
{
	float linearDepth = clamp(GetDepth(gl_FragCoord.z) * variables.ranges.y, 0.0, 25000.0) / 25000.0;
	//float modDepth = clamp(linearDepth + 0.15, 0.0, 1.0);
	float modDepth = clamp(linearDepth + 0.1, 0.0, 1.0);
	vec4 aerialPerspective = texture(aerialSampler, vec3(gl_FragCoord.xy / variables.resolution.xy, modDepth));
	vec4 aerialColor = vec4(aerialPerspective.rgb * LIGHT_COLOR * 4.0, aerialPerspective.a);

	return (aerialColor);
}

vec3 BlendNormal(vec2 uv, vec2 time, float viewDistance)
{
	vec3 result = vec3(0.0, 1.0, 0.0);
	/*WaveNormals result;
	result.skyNormal = vec3(0.0, 1.0, 0.0);
	result.specularNormal = vec3(0.0, 1.0, 0.0);

	float currentSkyPower = startSkyPower;
	float currentSpecularPower = startSpecularPower;
	vec3 currentNormal;*/

	float currentDistance = startDistance;
	float currentBlend = currentDistance * blendDistance;
	float currentScale = startScale;
	float currentSpeed = startSpeed;
	float currentPower = startPower;
	

	for (int i = 0; i < maxBlend; i++)
	{
		currentBlend = currentDistance * blendDistance;
		if (viewDistance <= currentDistance + currentBlend)
		{
			result = SampleNormal(uv * currentScale + time * currentSpeed, currentPower);
			/*currentNormal = SampleNormal(uv * currentScale + time * currentSpeed, 1.0);

			result.skyNormal = currentNormal;
			result.skyNormal.xz *= currentSkyPower;
			result.skyNormal = normalize(result.skyNormal);

			result.specularNormal = currentNormal;
			result.specularNormal.xz *= currentSpecularPower;
			result.specularNormal = normalize(result.specularNormal);*/

			if (viewDistance >= currentDistance - currentBlend)
			{
				float blendFactor = viewDistance - (currentDistance - currentBlend);
				blendFactor /= currentBlend * 2;

				result *= (1.0 - blendFactor);
				vec3 resultBlend = vec3(0.0, 1.0, 0.0);
				if (i + 1 < maxBlend) 
				{
					resultBlend = SampleNormal(uv * (currentScale * scaleIncrease) + (time * (currentSpeed * speedIncrease)), 
						(currentPower * powerIncrease));
				}
				result += resultBlend * blendFactor;
				
				/*vec3 currentNormalBlend = SampleNormal(uv * (currentScale * scaleIncrease) + (time * (currentSpeed * speedIncrease)), 1.0);

				vec3 currentSkyNormalBlend = currentNormalBlend;
				currentSkyNormalBlend.xz *= (currentSkyPower * skyPowerIncrease);
				currentSkyNormalBlend = normalize(currentSkyNormalBlend);
				result.skyNormal *= (1.0 - blendFactor);
				result.skyNormal += currentSkyNormalBlend * (blendFactor);

				vec3 currentSpecularNormalBlend = currentNormalBlend;
				currentSpecularNormalBlend.xz *= (currentSpecularPower * specularPowerIncrease);
				currentSpecularNormalBlend = normalize(currentSpecularNormalBlend);
				result.specularNormal *= (1.0 - blendFactor);
				result.specularNormal += currentSpecularNormalBlend * (blendFactor);*/
			}

			break;
		}
		currentDistance *= distanceIncrease;
		currentScale *= scaleIncrease;
		currentSpeed *= speedIncrease;
		currentPower *= powerIncrease;

		//currentSkyPower *= skyPowerIncrease;
		//currentSpecularPower *= specularPowerIncrease;
	}

	//result.skyNormal = normalize(result.skyNormal);
	//result.specularNormal = normalize(result.specularNormal);
	
	//currentBlend = currentDistance * blendDistance;
	//if (viewDistance <= currentDistance + currentBlend)
	//{
	//	float blendFactor = viewDistance - (maxDistance);
	//	blendFactor /= maxBlend;
	//	result *= (1.0 - blendFactor);
	//	result += defaultResult * blendFactor;
	//}

	result = normalize(result);
	return (result);
}

void main()
{
	float depth = subpassLoad(inputDepth).r;
	if (gl_FragCoord.z > depth) discard;
	
	float bottomDepth = GetDepth(depth);
	float surfaceDepth = GetDepth(gl_FragCoord.z);
	float waterDepth = (bottomDepth - surfaceDepth);
	//float fogFactor = clamp(exp2(-waterDensity * waterDepth), 0.25, 1.0) - 0.25;
	float fogFactor = clamp(exp2(-waterDensity * waterDepth), 0.0, 1.0);

	float shadow = 1.0 - GetTerrainShadow(worldPosition.xz);

	vec2 uv = worldPosition.xz + variables.terrainOffset.xz;
	float viewDistance = distance(variables.viewPosition, worldPosition);

	/*WaveNormals normals = BlendNormal(uv, vec2(variables.time), viewDistance);
	vec3 skyNormal = normals.skyNormal;
	vec3 specularNormal = normals.specularNormal;
	normals = BlendNormal(uv * 0.25, vec2(-variables.time), viewDistance);
	skyNormal += normals.skyNormal;
	specularNormal += normals.specularNormal;
	normals = BlendNormal(uv * 0.05, vec2(-variables.time, variables.time), viewDistance);
	skyNormal += normals.skyNormal;
	specularNormal += normals.specularNormal;
	normals = BlendNormal(uv * 2.5, vec2(variables.time, -variables.time), viewDistance);
	skyNormal += normals.skyNormal;
	specularNormal += normals.specularNormal;

	skyNormal = normalize(skyNormal);
	specularNormal = normalize(specularNormal);*/

	vec3 normal = BlendNormal(uv, vec2(variables.time), viewDistance);
	normal += BlendNormal(uv * 0.25, vec2(-variables.time), viewDistance);
	normal += BlendNormal(uv * 0.05, vec2(-variables.time, variables.time), viewDistance);
	normal += BlendNormal(uv * 2.5, vec2(variables.time, -variables.time), viewDistance);
	normal = normalize(normal);

	//float coastDistance = clamp(worldPosition.y - GetTerrainHeight(worldPosition.xz), 10.0, 50.0) / 50.0;
	float coastDistance = 1.0;

	vec3 skyNormal = normal;
	skyNormal.xz *= 12.0 * coastDistance;
	skyNormal = normalize(skyNormal);

	vec3 specularNormal = normal;
	specularNormal.xz *= 3.0;
	specularNormal = normalize(specularNormal);

	//vec3 currentSkyColor = GetSkyColor(skyNormal) * sunColor.rgb * 0.25;
	vec3 currentSkyColor = GetSkyColor(skyNormal) * sunColor.rgb * 0.5;
	vec4 aerialColor = GetAerialColor();

	vec3 viewDirection = normalize(variables.viewPosition - worldPosition);

	vec3 originalColor = subpassLoad(inputColor).rgb;

	vec3 diffuse = currentSkyColor * mix(0.5, 1.0, shadow);
	//vec3 specular = SpecularLighting(specularNormal, viewDirection, 128.0 * 8.0);
	vec3 specular = SpecularLighting(specularNormal, viewDirection, 128.0 * 8.0);

	vec3 finalColor = mix(diffuse, originalColor, fogFactor) + (specular * shadow);
	finalColor = mix(finalColor, aerialColor.rgb, 1.0 - aerialColor.a);

	outColor = vec4(finalColor, 1.0);
}