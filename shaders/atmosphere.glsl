#ifndef ATMOSPHERE_INCLUDED
#define ATMOSPHERE_INCLUDED

#define TRANSMITTANCE_ITERATIONS 400
#define MULTISCATTER_ITERATIONS 20
#define SCATTER_ITERATIONS 5
#define OPTICAL_ITERATIONS 5
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

const float RSH = 8000.0;
const float MSH = 1200.0;
const vec3 betaR = vec3(5.8e-6, 13.5e-6, 33.1e-6);
const vec3 betaM = vec3(3.996e-6);
const vec3 ME = vec3(4.440e-6);
const float PR = 6360000.0;
const float AR = 6460000.0;
const float AH = AR - PR;

struct RaymarchResult 
{
    vec3 luminance;
    vec3 multiscattering;
};

float SphereIntersectNearest(vec3 point, vec3 direction, float radius)
{
	vec3 offset = point - vec3(0);
	float a = dot(direction, direction);
	float b = 2.0 * dot(direction, offset);
	float c = dot(offset, offset) - (radius * radius);
	float discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0 || a == 0.0)
		return (-1.0);

	float t0 = (-b - sqrt(discriminant)) / (2.0 * a);
	float t1 = (-b + sqrt(discriminant)) / (2.0 * a);

	if (t0 < 0.0 && t1 < 0.0)
		return (-1.0);

	if (t0 < 0.0)
		return (max(0.0, t1));
	else if (t1 < 0.0)
		return (max(0.0, t0));

	return (max(0.0, min(t0, t1)));
}

vec2 SphereIntersect(vec3 point, vec3 direction)
{
	vec3 offset = point - vec3(0);
	float a = dot(direction, direction);
	float b = 2.0 * dot(direction, offset);
	float c = dot(offset, offset) - (AR * AR);
	float discriminant = b * b - 4.0 * a * c;

	if (discriminant < 0.0)
		return (vec2(0, 0));

	float t0 = (-b - sqrt(discriminant)) / (2.0 * a);
	float t1 = (-b + sqrt(discriminant)) / (2.0 * a);

	return (vec2(t0, t1));
}

float ScatterValues(float u, float resolution)
{
	return ((u - 0.5 / resolution) * (resolution / (resolution - 1.0)));
}

vec2 TransmittanceUV(vec2 values)
{
	float atmosphereHeight = sqrt(AR * AR - PR * PR);
	float uvHeight = sqrt(values.x * values.x - PR * PR);

	float discriminant = values.x * values.x * (values.y * values.y - 1.0) + AR * AR;
	float atmosphereDistance = max(0.0, (-values.x * values.y + sqrt(discriminant)));
	float minDistance = AR - values.x;
	float maxDistance = uvHeight + atmosphereHeight;
	float mu = (atmosphereDistance - minDistance) / (maxDistance - minDistance);
	float r = uvHeight / atmosphereHeight;

	return (vec2(mu, r));
}

vec2 TransmittanceValues(vec2 uv)
{
	float atmosphereHeight = sqrt(AR * AR - PR * PR);
	float uvHeight = uv.y * atmosphereHeight;
	float x = sqrt(uvHeight * uvHeight + PR * PR);

	float minDistance = AR - x;
	float maxDistance = uvHeight + atmosphereHeight;
	float uvDistance = minDistance + uv.x * (maxDistance - minDistance);
	float y = 1.0;
	if (uvDistance != 0.0)
		y = (atmosphereHeight * atmosphereHeight - uvHeight * uvHeight - uvDistance * uvDistance) / (2.0 * x * uvDistance);
	y = clamp(y, -1.0, 1.0);

	return (vec2(x, y));
}

float Density(float height, float scale)
{
	return (exp(-height / scale));
}

#ifdef SCATTER_COMPUTE

/*vec3 MultiScatter(vec3 start, vec3 end)
{
	vec3 ray = start;
	vec3 rayStep = (end - start) / float(MULTISCATTER_ITERATIONS - 1.0);
	float rayStepSize = length(rayStep);
	vec3 accumulatedScattering = vec3(0.0);

	for (int i = 0; i < MULTISCATTER_ITERATIONS; i++)
	{
		float height = length(ray) - PR;
		float betaE = Density(height, RSH) + Density(height, MSH);
		vec3 transmittance = textureLod(transmittanceSampler, vec2(height / AH, 0.5), 0.0).rgb;
		accumulatedScattering += betaE * transmittance * rayStepSize;
		ray += rayStep;
	}

	return (accumulatedScattering);
}*/

RaymarchResult MultiScatter(vec3 rayStart, vec3 rayDirection, vec3 sunDirection)
{
	RaymarchResult result = RaymarchResult(vec3(0.0), vec3(0.0));

	float atmosphereDistance = SphereIntersectNearest(rayStart, rayDirection, AR);
	float surfaceDistance = SphereIntersectNearest(rayStart, rayDirection, PR);

	float rayLength;

	if (surfaceDistance == -1.0 && atmosphereDistance == -1.0)
		return (result);
	else if (surfaceDistance == -1.0 && atmosphereDistance > 0.0)
		rayLength = atmosphereDistance;
	else if (surfaceDistance > 0.0 && atmosphereDistance == -1.0)
		rayLength = surfaceDistance;
	else
		rayLength = min(surfaceDistance, atmosphereDistance);

	float rayStep = rayLength / float(MULTISCATTER_ITERATIONS);

	vec3 accumulatedTransmittance = vec3(0.0);
	vec3 accumulatedLuminance = vec3(0.0);
	float oldRayShift = 0.0;

	for (int i = 0; i < MULTISCATTER_ITERATIONS; i++)
	{
		float newRayShift = rayLength * (float(i) + 0.3) / float(MULTISCATTER_ITERATIONS);
		rayStep = newRayShift - oldRayShift;
		vec3 ray = rayStart + newRayShift * rayDirection;
		oldRayShift = newRayShift;

		vec3 upVector = normalize(ray);
		vec2 transmittanceValues = vec2(length(ray), dot(sunDirection, upVector));
		vec2 transmittanceUV = TransmittanceUV(transmittanceValues);

		vec3 sunTransmittance = textureLod(transmittanceSampler, transmittanceUV, 0.0).rgb;
		vec3 scattering; //continue from here!!!!
	}
}

#endif

vec3 Extinction(vec3 point)
{
	float height = length(point) - PR;

	float mieDensity = exp(-height / MSH);
	float rayDensity = exp(-height / RSH);

	vec3 mieExtinction = mieDensity * ME;
	vec3 rayExtinction = rayDensity * betaR;

	return (mieExtinction + rayExtinction);
}

vec3 Transmittance(vec3 start, vec3 direction)
{
	float rayLength = SphereIntersectNearest(start, direction, AR);
	float rayStep = rayLength / float(TRANSMITTANCE_ITERATIONS);
	vec3 totalTransmittance = vec3(0.0);

	for (int i = 0; i < TRANSMITTANCE_ITERATIONS; i++)
	{
		vec3 ray = start + i * rayStep * direction;
		vec3 extinction = Extinction(ray);
		totalTransmittance += extinction * rayStep;
	}

	return (totalTransmittance);
}

/*vec3 Transmittance(vec3 start, vec3 end)
{
	vec3 ray = start;
	vec3 rayStep = (end - start) / float(TRANSMITTANCE_ITERATIONS);
	float rayStepSize = length(rayStep);
	vec3 tau = vec3(0.0);

	for (int i = 0; i < TRANSMITTANCE_ITERATIONS; i++)
	{
		ray += rayStep;
		float height = length(ray) - PR;
		vec3 betaE = betaR * Density(height, RSH) + betaM * Density(height, MSH);
		tau += betaE * rayStepSize;
	}

	return (exp(-tau));
}*/

vec3 sunWithBloom(vec3 rayDir, vec3 sunDir)
{
    const float sunSolidAngle = 1*PI/180.0;
    const float minSunCosTheta = cos(sunSolidAngle);

    float cosTheta = dot(rayDir, sunDir);
    if (cosTheta >= minSunCosTheta) return vec3(1.0);
    
    float offset = minSunCosTheta - cosTheta;
    float gaussianBloom = exp(-offset * 50000.0 * 16.0)*0.5;
    float invBloom = 4.0 / (0.02 + offset * 100.0) * 0.01;
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