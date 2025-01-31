#ifndef ATMOSPHERE_INCLUDED
#define ATMOSPHERE_INCLUDED

#define SCATTER_ITERATIONS 10
#define OPTICAL_ITERATIONS 10
#define BAKED_OPTICAL_ITERATIONS 50
#define PI 3.14159265359

const float sunCenter = 0.1;
const float outer = 1.0 - sunCenter;

const float mid = 0.75;
//float inside = 1.0 - outside;

const float radius = 10250000;
const float radiusSqr = radius * radius;
const float surface = 10000000;
const float surfaceOffset = 162000.0;
const float surfaceDepth = -surface - surfaceOffset;
const float surfaceSqr = surface * surface;
const vec3 surfaceCenter = vec3(0, surfaceDepth, 0);

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

/*vec3 GetSky()
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
}*/

vec2 AtmosphereIntersect(vec3 point, vec3 direction)
{
	vec3 offset = point - surfaceCenter;
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

float GetHeight01(vec3 point)
{
	float height = distance(point, surfaceCenter) - surface;
	float height01 = clamp(height * airMult, 0.0, 1.0);

	return (height01);
}

float GetDensity(vec3 point)
{
	//float height = distance(point, surfaceCenter) - surface;
	//float height01 = clamp(height * airMult, 0.0, 1.0);
	float height01 = GetHeight01(point);
	float localDensity = exp(-height01 * densityFalloff) * (1.0 - height01);

	return (localDensity);
}

float GetDensity(vec3 point, vec3 center)
{
	float height = length(point - center) - surface;
	float height01 = clamp(height * airMult, 0.0, 1.0);
	float localDensity = exp(-height01 * densityFalloff) * (1.0 - height01);

	return (localDensity);
}

float GetOpticalDepth(vec3 point, vec3 direction, float len)
{
	vec3 opticalPoint = point;
	float opticalDepth = 0.0;
	float stepSize = len / (BAKED_OPTICAL_ITERATIONS - 1);
	//float stepSizeDensity = 1.0 / (BAKED_OPTICAL_ITERATIONS - 1);

	for (int i = 0; i < BAKED_OPTICAL_ITERATIONS; i++)
	{
		opticalDepth += GetDensity(opticalPoint) * stepSize;
		opticalPoint += direction * stepSize;
	}

	return (opticalDepth);
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

#endif