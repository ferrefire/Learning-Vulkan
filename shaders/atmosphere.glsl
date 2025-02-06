#ifndef ATMOSPHERE_INCLUDED
#define ATMOSPHERE_INCLUDED

#define TRANSMITTANCE_ITERATIONS 400
#define MULTISCATTER_ITERATIONS 20
#define SKY_VIEW_ITERATIONS 30

//maybe change to 10.0
#ifndef RADIUS_OFFSET
#define RADIUS_OFFSET 0.01
//#define RADIUS_OFFSET 10.0
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

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

//const float RHS = 8000.0;
//const float MHS = 1200.0;
//const vec3 RS = vec3(5.8e-6, 13.5e-6, 33.1e-6);
//const vec3 MS = vec3(3.996e-6);
//const vec3 ME = vec3(4.440e-6);
//const float PR = 6360000.0;
//const float AR = 6460000.0;
//const float AH = AR - PR;

const float RHS = 8.0;
const float MHS = 1.2 * 4.0;
const vec3 RS = vec3(0.005802, 0.013558, 0.033100);
const vec3 MS = vec3(0.003996, 0.003996, 0.003996) * 1.0;
const vec3 ME = vec3(0.004440, 0.004440, 0.004440) * 1.0;
const float MP = 0.8;
const float PR = 6360.0;
const float AR = 6460.0;
const float AH = AR - PR;

struct RaymarchResult 
{
    vec3 luminance;
    vec3 multiScattering;
};

struct ScatterResult 
{
    vec3 mie;
    vec3 rayleigh;
};

vec3 sunWithBloom(vec3 worldDir, vec3 sunDir)
{
    const float sunSolidAngle = 1.0 * PI / 180.0;
    const float minSunCosTheta = cos(sunSolidAngle);

    float cosTheta = dot(worldDir, sunDir);
    if(cosTheta >= minSunCosTheta) {return vec3(0.5) ;}
    float offset = minSunCosTheta - cosTheta;
    float gaussianBloom = exp(-offset * 50000.0) * 0.5;
    float invBloom = 1.0/(0.02 + offset * 300.0) * 0.01;
    return vec3(gaussianBloom + invBloom);
}

float SphereIntersectNearest(vec3 point, vec3 direction, float radius, vec3 centerOffset)
{
	vec3 offset = point - centerOffset;
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

float SphereIntersectNearest(vec3 point, vec3 direction, float radius)
{
	return (SphereIntersectNearest(point, direction, radius, vec3(0.0)));
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

bool MoveToAtmosphereEdge(inout vec3 point, vec3 direction)
{
	if (length(point) > AR)
	{
		float atmosphereDistance = SphereIntersectNearest(point, direction, AR);

		if (atmosphereDistance == -1.0)
		{
			return (false);
		}
		else
		{
			vec3 upOffset = normalize(point) * -RADIUS_OFFSET;
			point += direction * atmosphereDistance + upOffset;
		}
	}

	return (true);
}

float ScatterUV(float u, float resolution)
{
	return ((u + 0.5 / resolution) * (resolution / (resolution + 1.0)));
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

vec2 ViewUV(vec2 values, vec2 dimensions, float height, bool surfaceIntersect)
{
	vec2 uv;

	float beta = asin(PR / height);
	float horizonAngle = PI - beta;

	if (!surfaceIntersect)
	{
		float coord = values.x / horizonAngle;
		coord = (1.0 - sqrt(1.0 - coord)) / 2.0;
		uv.y = coord;
	}
	else
	{
		float coord = (values.x - horizonAngle) / beta;
		coord = (sqrt(coord) + 1.0) / 2.0;
		uv.y = coord;
	}

	uv.x = sqrt(values.y / PI);

	uv = vec2(ScatterUV(uv.x, dimensions.x), ScatterUV(uv.y, dimensions.y));

	return (uv);
}

vec2 ViewValues(vec2 uv, vec2 dimensions, float height)
{
	vec2 values = vec2(ScatterValues(uv.x, dimensions.x), ScatterValues(uv.y, dimensions.y));

	float beta = asin(PR / height);
	float horizonAngle = PI - beta;

	float viewAngle;
	float lightAngle;

	if (uv.y < 0.5)
	{
		float coord = 1.0 - (1.0 - 2.0 * uv.y) * (1.0 - 2.0 * uv.y);
		viewAngle = horizonAngle * coord;
	}
	else
	{
		float coord = (uv.y * 2.0 - 1.0) * (uv.y * 2.0 - 1.0);
		viewAngle = horizonAngle + beta * coord;
	}

	lightAngle = (uv.x * uv.x) * PI;

	return (vec2(viewAngle, lightAngle));
}

float Density(float height, float scale)
{
	return (exp(-height / scale));
}

ScatterResult ScatterResults(vec3 point)
{
	float height = length(point) - PR;

	float mieDensity = exp(-height / MHS);
	float rayDensity = exp(-height / RHS);

	vec3 mieScattering = mieDensity * MS;
	vec3 rayScattering = rayDensity * RS;

	return ScatterResult(mieScattering, rayScattering);
}

vec3 Scatter(vec3 point)
{
	ScatterResult result = ScatterResults(point);

	return (result.mie + result.rayleigh);
}

vec3 Extinction(vec3 point)
{
	float height = length(point) - PR;

	float mieDensity = exp(-height / MHS);
	float rayDensity = exp(-height / RHS);

	vec3 mieExtinction = mieDensity * ME;
	vec3 rayExtinction = rayDensity * RS;

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

#ifdef SCATTER_COMPUTE

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

	vec3 accumulatedTransmittance = vec3(1.0);
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
		vec3 scattering = Scatter(ray);
		vec3 extinction = Extinction(ray);

		vec3 transmittanceIncrease = exp(-(extinction * rayStep));
		float surfaceDistance = SphereIntersectNearest(ray, sunDirection, PR, RADIUS_OFFSET * upVector);
		float inShadow = surfaceDistance == -1.0 ? 1.0 : 0.0;

		vec3 light = inShadow * sunTransmittance * scattering * uniformPhase;
		vec3 multiScatterIn = (scattering - scattering * transmittanceIncrease) / extinction;
		vec3 scatterIn = (light - light * transmittanceIncrease) / extinction;

		if (all(equal(transmittanceIncrease, vec3(1.0))))
		{
			multiScatterIn = vec3(0.0);
			scatterIn = vec3(0.0);
		}

		result.multiScattering += accumulatedTransmittance * multiScatterIn;
		accumulatedLuminance += accumulatedTransmittance * scatterIn;
		accumulatedTransmittance *= transmittanceIncrease;
	}

	result.luminance = accumulatedLuminance;

	return (result);
}

#endif

#if defined(VIEW_COMPUTE) || defined(AERIAL_COMPUTE)

float MiePhase(float g, float cosTheta)
{
	float k = 3.0 / (8.0 * PI) * (1.0 - g * g) / (2.0 + g * g);
	return (k * (1.0 + cosTheta * cosTheta) / pow(1.0 + g * g - 2.0 * g * -cosTheta, 1.5));
}

float RayPhase(float cosTheta)
{
	float factor = 3.0 / (16.0 * PI);
	return (factor * (1.0 + cosTheta * cosTheta));
}

vec3 MultiScatter(vec3 point, float viewAngle)
{
	vec2 values = clamp(vec2(viewAngle * 0.5 + 0.5, (length(point) - PR) / (AR - PR)), 0.0, 1.0);
	vec2 uv = vec2(ScatterUV(values.x, scatterResolution.x), ScatterUV(values.y, scatterResolution.y));

	return (textureLod(scatterSampler, uv, 0).rgb);
}

#endif

#ifdef VIEW_COMPUTE

vec3 Luminance(vec3 rayStart, vec3 rayDirection, vec3 sunDirection)
{
	float atmosphereDistance = SphereIntersectNearest(rayStart, rayDirection, AR);
	float surfaceDistance = SphereIntersectNearest(rayStart, rayDirection, PR);

	float rayLength;

	if (surfaceDistance == -1.0 && atmosphereDistance == -1.0)
		return (vec3(0.0));
	else if (surfaceDistance == -1.0 && atmosphereDistance > 0.0)
		rayLength = atmosphereDistance;
	else if (surfaceDistance > 0.0 && atmosphereDistance == -1.0)
		rayLength = surfaceDistance;
	else
		rayLength = min(surfaceDistance, atmosphereDistance);

	float cosTheta = dot(sunDirection, rayDirection);
	float miePhaseValue = MiePhase(MP, -cosTheta);
	float rayPhaseValue = RayPhase(cosTheta);

	vec3 accumulatedTransmittance = vec3(1.0);
	vec3 accumulatedLuminance = vec3(0.0);

	for (int i = 0; i < SKY_VIEW_ITERATIONS; i++)
	{
		float step0 = float(i) / SKY_VIEW_ITERATIONS;
		float step1 = float(i + 1) / SKY_VIEW_ITERATIONS;

		step0 *= step0;
		step1 *= step1;

		step0 = step0 * rayLength;
		step1 = step1 > 1.0 ? rayLength : step1 * rayLength;

		float rayStep = step0 + (step1 - step0) * 0.3;
		float stepDiff = step1 - step0;

		vec3 ray = rayStart + rayDirection * rayStep;
		ScatterResult scattering = ScatterResults(ray);
		vec3 extinction = Extinction(ray);

		vec3 upVector = normalize(ray);
		vec2 transmittanceValues = vec2(length(ray), dot(sunDirection, upVector));
		vec2 transmittanceUV = TransmittanceUV(transmittanceValues);

		vec3 sunTransmittance = textureLod(transmittanceSampler, transmittanceUV, 0.0).rgb;
		vec3 phaseScattering = scattering.mie * miePhaseValue + scattering.rayleigh * rayPhaseValue;

		float surfaceDistance = SphereIntersectNearest(ray, sunDirection, PR, RADIUS_OFFSET * upVector);
		float inShadow = surfaceDistance == -1.0 ? 1.0 : 0.0;

		vec3 multiScatteredLuminance = MultiScatter(ray, dot(sunDirection, upVector));

		vec3 light = inShadow * sunTransmittance * phaseScattering + multiScatteredLuminance * (scattering.rayleigh + scattering.mie);

		vec3 transmittanceIncrease = exp(-(extinction * stepDiff));
		vec3 incomingLight = (light - light * transmittanceIncrease) / extinction;
		accumulatedLuminance += accumulatedTransmittance * incomingLight;
		accumulatedTransmittance *= transmittanceIncrease;
	}

	return (accumulatedLuminance);
}

#endif

#ifdef AERIAL_COMPUTE

RaymarchResult AerialScatter(vec3 rayStart, vec3 rayDirection, vec3 sunDirection, int sampleCount, float maxDis)
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
	rayLength = min(rayLength, maxDis);
	
	float cosTheta = dot(sunDirection, rayDirection);
	float miePhaseValue = MiePhase(MP, -cosTheta);
	float rayPhaseValue = RayPhase(cosTheta);

	vec3 accumulatedTransmittance = vec3(1.0);
	vec3 accumulatedLuminance = vec3(0.0);
	float oldRayShift = 0.0;
	float rayStep = 0.0;

	for (int i = 0; i < sampleCount; i++)
	{
		float newRayShift = rayLength * (float(i) + 0.3) / float(sampleCount);
		rayStep = newRayShift - oldRayShift;
		vec3 ray = rayStart + newRayShift * rayDirection;
		oldRayShift = newRayShift;

		ScatterResult scattering = ScatterResults(ray);
		vec3 extinction = Extinction(ray);

		vec3 upVector = normalize(ray);
		vec2 transmittanceValues = vec2(length(ray), dot(sunDirection, upVector));
		vec2 transmittanceUV = TransmittanceUV(transmittanceValues);

		vec3 sunTransmittance = textureLod(transmittanceSampler, transmittanceUV, 0.0).rgb;
		vec3 phaseScattering = scattering.mie * miePhaseValue + scattering.rayleigh * rayPhaseValue;

		float surfaceDistance = SphereIntersectNearest(ray, sunDirection, PR, RADIUS_OFFSET * upVector);
		float inShadow = surfaceDistance == -1.0 ? 1.0 : 0.0;

		vec3 multiScatteredLuminance = MultiScatter(ray, dot(sunDirection, upVector));

		vec3 light = inShadow * sunTransmittance * phaseScattering + multiScatteredLuminance * (scattering.rayleigh + scattering.mie);

		vec3 transmittanceIncrease = exp(-(extinction * rayStep));
		vec3 incomingLight = (light - light * transmittanceIncrease) / extinction;
		accumulatedLuminance += accumulatedTransmittance * incomingLight;
		accumulatedTransmittance *= transmittanceIncrease;
	}

	result.luminance = accumulatedLuminance;
	result.multiScattering = accumulatedTransmittance;

	return (result);
}

#endif

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
		vec3 betaE = RS * Density(height, RHS) + MS * Density(height, MHS);
		tau += betaE * rayStepSize;
	}

	return (exp(-tau));
}

vec3 sunWithBloom(vec3 rayDir, vec3 sunDir)
{
    float sunSolidAngle = 1.0 * PI / 180.0;
    float minSunCosTheta = cos(sunSolidAngle);

    float cosTheta = dot(rayDir, sunDir);
    if (cosTheta >= minSunCosTheta) return vec3(1.0);
    
    float offset = minSunCosTheta - cosTheta;
    float gaussianBloom = exp(-offset * 50000.0 * 16.0)*0.5;
    float invBloom = 4.0 / (0.02 + offset * 100.0) * 0.01;
    //return vec3(gaussianBloom+invBloom);
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
}*/

#endif