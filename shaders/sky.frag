#version 450

#extension GL_ARB_shading_language_include : require

layout (input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "transformation.glsl"
#include "depth.glsl"
#include "lighting.glsl"

#define SCATTER_ITERATIONS 5
#define OPTICAL_ITERATIONS 5
#define PI 3.14159265359

float sunCenter = 0.1;
float outer = 1.0 - sunCenter;

float mid = 0.75;
//float inside = 1.0 - outside;

const float radius = 10250000;
//const float radius = 20000;
const float radiusSqr = radius * radius;
//const float surface = 5000;
const float surface = 10000000;
const float surfaceSqr = surface * surface;
const float densityFalloff = 10.0;
const float mistFalloff = 0.25;

const float airMult = 1.0 / (radius - surface);

const float rayleighHeightScale = 5000;
const float mieHeightScale = 1000;
//const vec3 rayleighBeta = vec3(680, 550, 440);
const vec3 rayleighBeta = vec3(0.0058, 0.0135, 0.0331);
const vec3 mieBeta = vec3(0.01);

vec3 sunWithBloom(vec3 rayDir, vec3 sunDir)
{
    const float sunSolidAngle = 1*PI/180.0;
    const float minSunCosTheta = cos(sunSolidAngle);

    float cosTheta = dot(rayDir, sunDir);
    if (cosTheta >= minSunCosTheta) return vec3(1.0);
    
    float offset = minSunCosTheta - cosTheta;
    float gaussianBloom = exp(-offset * 50000.0 * 16.0)*0.5;
    float invBloom = 16.0 / (0.02 + offset * 300.0) * 0.01;
   // return vec3(gaussianBloom+invBloom);
    return LIGHT_COLOR * (gaussianBloom + invBloom);
}

vec3 GetSky()
{
	vec3 finalColor = SKY_COLOR;

	vec2 lightScreenPosition = WorldToClip(variables.viewPosition + variables.lightDirection).xy;
	vec2 skyPosition = inCoordinates;

	float intensity = pow(1.0 - clamp(length((lightScreenPosition - skyPosition) * vec2(1.0, 0.5625)), 0.0, 1.0), 4.0);
	if (intensity > 0.001)
	{
		float blendIntensity = clamp(intensity, 0.0, mid) / mid;
		finalColor = mix(vec3(1.0, 0.8, 0.0), SKY_COLOR, pow(1.0 - blendIntensity, 1.0));
		if (intensity > sunCenter)
		{
			intensity = (intensity - sunCenter) / outer;
			intensity = 1.0 - pow(1.0 - intensity, 1.0);
			finalColor = mix(finalColor, vec3(1), intensity);
		}
	}

	return (finalColor);
}

vec2 AtmosphereIntersect(vec3 point, vec3 direction, vec3 center)
{
	vec3 offset = point - center;
	float a = dot(direction, direction);
	float b = 2.0 * dot(offset, direction);
	float c = dot(offset, offset) - radiusSqr;
	float discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
		return (vec2(0, 0));

	float t0 = (-b - sqrt(discriminant)) / (2.0 * a);
	float t1 = (-b + sqrt(discriminant)) / (2.0 * a);
	float tStart = max(0.0, t0);
	float tEnd = t1;

	return (vec2(tStart, tEnd));
}

float GetDensity(vec3 point, vec3 center)
{
	float height = length(point - center) - surface;
	//float height01 = height / (radius - surface);
	float height01 = clamp(height * airMult, 0.0, 1.0);
	//float localDensity = exp(-height01 / (rayleighHeightScale * airMult)) * (1.0 - height01);
	float localDensity = exp(-height01 * densityFalloff) * (1.0 - height01);
	return (localDensity);
	//float rayHeight = exp(-height / rayleighHeightScale);
	//float mieHeight = exp(-height / mieHeightScale);
}

float GetOpticalDepth(vec3 point, vec3 center, vec3 direction, float len)
{
	vec3 opticalPoint = point;
	float opticalDepth = 0.0;
	float stepSize = len / (OPTICAL_ITERATIONS - 1);

	for (int i = 0; i < OPTICAL_ITERATIONS; i++)
	{
		opticalDepth += GetDensity(opticalPoint, center) * stepSize;
		opticalPoint += direction * stepSize;
	}

	return (opticalDepth);
}

vec3 GetScattering(float depth, vec3 originalColor)
{
	vec4 screenPosition;
	screenPosition.xy = (((gl_FragCoord.xy + 0.5) / (variables.resolution.xy)) - 0.5) * 2.0;
	screenPosition.z = 0.0;
	screenPosition.w = 1.0;

	//center.y = 0;
	
	vec4 pixelPosition = inverse(variables.viewMatrix) * screenPosition;
	pixelPosition /= pixelPosition.w;

	vec3 scatterPosition = variables.viewPosition;

	vec3 pixelDirection = normalize(pixelPosition.xyz - variables.viewPosition);
	vec3 pixelStart = variables.viewPosition;

	vec3 surfaceCenter = variables.viewPosition;
	surfaceCenter.y = -surface - 162000.0;

	vec2 tRange = AtmosphereIntersect(pixelStart, pixelDirection, surfaceCenter);

	if (tRange.x == 0 && tRange.y == 0) return (vec3(1));

	float rayLength = tRange.y;
	if (depth >= 0.0 && depth < 1.0) rayLength = min(tRange.y, depth * variables.ranges.y * 1.0);

	vec3 totalRayScattering = vec3(0.0);
	vec3 totalMieScattering = vec3(0.0);
	float totalOpticalDepth = 0.0;
	float viewOptical = 0.0;

	float stepSize = rayLength / (SCATTER_ITERATIONS - 1);

	for (int i = 0; i < SCATTER_ITERATIONS; i++)
	{
		//float t = mix(tRange.x, tRange.y, float(i) / (SCATTER_ITERATIONS - 1));
		//vec3 scatterPosition = pixelStart + (t * pixelDirection);

		float sunLen = AtmosphereIntersect(scatterPosition, variables.lightDirection, surfaceCenter).y;
		float sunOptical = GetOpticalDepth(scatterPosition, surfaceCenter, variables.lightDirection, sunLen);
		viewOptical = GetOpticalDepth(scatterPosition, surfaceCenter, -pixelDirection, i * stepSize);
		vec3 transmittance = exp(-(sunOptical + viewOptical) * rayleighBeta);
		float localDensity = GetDensity(scatterPosition, surfaceCenter);

		//totalRayScattering += localDensity * transmittance * rayleighBeta * stepSize;
		totalRayScattering += localDensity * transmittance * rayleighBeta * stepSize;

		scatterPosition += pixelDirection * stepSize;

		//vec3 rayScatter = rayHeight * rayleighBeta;
		//vec3 mieScatter = mieHeight * mieBeta;
		//totalRayScattering += rayScatter * stepSize;
		//totalMieScattering += mieScatter * stepSize;
		//float opticalDepth = rayHeight * dot(rayleighBeta, vec3(1.0)) + mieHeight * dot(mieBeta, vec3(1.0));
		//totalOpticalDepth += opticalDepth * stepSize;
	}
	float originalTransmittance = exp(-viewOptical * mistFalloff);
	totalRayScattering = originalColor * originalTransmittance + totalRayScattering;
	//vec3 rayInScattering = totalRayScattering * exp(-totalOpticalDepth);
	//vec3 mieInScattering = totalMieScattering * exp(-totalOpticalDepth);

	if (depth >= 0.95 && dot(pixelDirection, variables.lightDirection) > 0.75)
	{
		totalRayScattering += sunWithBloom(pixelDirection, variables.lightDirection);
	}

	return (totalRayScattering);
	//return (totalRayScattering + totalMieScattering);
	//return (rayInScattering + mieInScattering);
}

void main()
{
	//vec3 finalColor = GetSky();

	//vec3 finalColor = GetScattering();

	//discard;

	vec3 finalColor = vec3(1.0);
	float depth = GetDepth(subpassLoad(inputDepth).r);

	//if (depth < 1.0 && depth * variables.ranges.y > 20000.0)
	//{
	//	outColor = vec4(0);
	//	return;
	//}

	vec3 scatteredLight;

	if (depth < 0.95)
	{
		vec3 originalColor = subpassLoad(inputColor).rgb;
		scatteredLight = GetScattering(depth, originalColor);
		finalColor = scatteredLight;
		//finalColor = Fog(originalColor, scatteredLight * 4.0, 1.0 - pow(1.0 - depth, 4.0));
		//finalColor = Fog(originalColor, scatteredLight, depth);

		//if (depth > 0.8)
		//{
		//	finalColor = mix(finalColor, GetScattering(1.0), pow((depth - 0.8) * 5.0, 1.5));
		//}
	}
	else
	{
		scatteredLight = GetScattering(1.0, vec3(0));
		finalColor = scatteredLight;
	}

	//vec3 finalColor = vec3(inCoordinates, 1.0);

	//vec3 finalColor = subpassLoad(inputColor).rgb;

    outColor = vec4(finalColor, 1.0);
}