#version 450

#extension GL_ARB_shading_language_include : require

layout (input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput inputDepth;
layout (set = 1, binding = 2) uniform sampler2D viewSampler;
layout (set = 1, binding = 3) uniform sampler3D aerialSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

const vec2 scatterResolution = vec2(32, 32);

#include "variables.glsl"
#include "transformation.glsl"
#include "depth.glsl"
#include "lighting.glsl"
#include "atmosphere.glsl"

/*vec3 GetScattering(float depth, vec3 originalColor)
{
	vec4 screenPosition;
	screenPosition.xy = (((gl_FragCoord.xy + 0.5) / (variables.resolution.xy)) - 0.5) * 2.0;
	screenPosition.z = 0.0;
	screenPosition.w = 1.0;
	
	vec4 pixelPosition = inverse(variables.viewMatrix) * screenPosition;
	pixelPosition /= pixelPosition.w;

	vec3 pixelDirection = normalize(pixelPosition.xyz - variables.viewPosition);
	vec3 pixelStart = vec3(0, variables.viewPosition.y - variables.terrainOffset.y, 0);

	vec3 scatterPosition = pixelStart;

	vec2 tRange = AtmosphereIntersect(pixelStart, pixelDirection);

	if (tRange.x == 0 && tRange.y == 0) return (vec3(1));

	float rayLength = tRange.y;
	if (depth >= 0.0 && depth < 1.0) rayLength = min(tRange.y, depth * variables.ranges.y * 1.0);

	vec3 totalRayScattering = vec3(0.0);
	float totalOpticalDepth = 0.0;
	float viewOptical = 0.0;

	float stepSize = rayLength / (SCATTER_ITERATIONS - 1);

	for (int i = 0; i < SCATTER_ITERATIONS; i++)
	{
		float sunLen = AtmosphereIntersect(scatterPosition, variables.lightDirection).y;
		float sunOptical = GetOpticalDepth(scatterPosition, variables.lightDirection, sunLen);

		//float currentHeight01 = GetHeight01(scatterPosition);
		////float currentAngle = (dot(variables.lightDirection, vec3(0, 1, 0)) + 1.0) * 0.5;
		//float currentAngle = dot(normalize(surfaceCenter - scatterPosition), variables.lightDirection) * 0.5 + 0.5;
		//float sunOptical = textureLod(scatterSampler, vec2(currentAngle, currentHeight01), 0).r * (radius - surface);

		viewOptical = GetOpticalDepth(scatterPosition, -pixelDirection, i * stepSize);

		vec3 transmittance = exp(-(sunOptical + viewOptical) * rayleighBeta);
		float localDensity = GetDensity(scatterPosition);

		totalRayScattering += localDensity * transmittance * rayleighBeta * stepSize;

		scatterPosition += pixelDirection * stepSize;
	}

	float originalTransmittance = exp(-viewOptical * mistFalloff);
	totalRayScattering = originalColor * originalTransmittance + totalRayScattering;

	if (depth >= 1.0 && dot(pixelDirection, variables.lightDirection) > 0.0)
	{
		totalRayScattering += sunWithBloom(pixelDirection, variables.lightDirection);
	}

	return (totalRayScattering);
}*/

void main()
{
	//outColor = vec4(subpassLoad(inputColor).rgb, 1.0);
	//return;

	/*vec3 finalColor = vec3(1.0);
	float depth = GetDepth(subpassLoad(inputDepth).r);

	vec3 scatteredLight;

	if (depth < 1.0)
	{
		vec3 originalColor = subpassLoad(inputColor).rgb;
		scatteredLight = GetScattering(depth, originalColor);
		finalColor = scatteredLight;
	}

	if (depth >= 0.95)
	{
		scatteredLight = GetScattering(1.0, vec3(0));
		finalColor = mix(finalColor, scatteredLight, (depth - 0.95) / 0.05);
	}

    outColor = vec4(finalColor, 1.0);*/

	//vec4 screenPosition;
	//screenPosition.xy = (((gl_FragCoord.xy + 0.5) / (variables.resolution.xy)) - 0.5) * 2.0;
	//screenPosition.z = 0.0;
	//screenPosition.w = 1.0;
	//vec4 pixelPosition = inverse(variables.viewMatrix) * screenPosition;
	//pixelPosition /= pixelPosition.w;
	//vec3 rayDirection = normalize(pixelPosition.xyz - variables.viewPosition);
	//rayDirection = normalize(Rotate(rayDirection, -90.0, vec3(1.0, 0.0, 0.0)));

	vec3 finalColor = vec3(0.0);

	float depth = subpassLoad(inputDepth).r;
	float linearDepth = GetDepth(depth);

	vec2 pixelPosition = inCoordinates;
	vec3 clipSpace = vec3(pixelPosition * vec2(2.0) - vec2(1.0), 1.0);
	vec4 hPos = inverse(variables.viewMatrix) * vec4(clipSpace, 1.0);
	//vec3 worldPosition = hPos.xyz / hPos.w;
	vec3 rayDirection = normalize(hPos.xyz / hPos.w - variables.viewPosition);
	rayDirection = normalize(Rotate(rayDirection, radians(-90.0), vec3(1.0, 0.0, 0.0)));
	vec3 rayStart = vec3(0.0, 0.0, PR + RADIUS_OFFSET + (variables.viewPosition.y - variables.terrainOffset.y) * 0.001);

	float viewHeight = length(rayStart);

	

	if (depth == 1.0)
	{
		vec2 uv;
		vec3 upVector = normalize(rayStart);
		float viewAngle = acos(dot(rayDirection, upVector));

		vec3 sunDirection = variables.rotatedLightDirection;

		float lightAngle = acos(dot(normalize(vec3(sunDirection.xy, 0.0)), normalize(vec3(rayDirection.xy, 0.0))));
		bool surfaceIntersect = SphereIntersectNearest(rayStart, rayDirection, PR) >= 0.0;

		uv = ViewUV(vec2(viewAngle, lightAngle), scatterResolution, viewHeight, surfaceIntersect);

		vec3 skyColor = texture(viewSampler, uv).rgb;

		vec3 incomingLight = skyColor;

		if (!surfaceIntersect)
		{
			incomingLight += sunWithBloom(rayDirection, sunDirection);
		}

		finalColor = incomingLight;
	}
	else
	{
		vec3 originalColor = subpassLoad(inputColor).rgb;

		if (linearDepth < 0.01)
		{
			finalColor = originalColor;
		}
		else
		{
			vec4 aerialPerspective = texture(aerialSampler, vec3(inCoordinates, linearDepth));
			vec3 aerialColor = aerialPerspective.rgb;
			float aerialDensity = clamp((length(aerialColor)) * 4.0, 0.0, 1.0);
			finalColor = mix(originalColor, aerialColor, linearDepth);
			//finalColor = originalColor * aerialPerspective.a + aerialColor;
			//finalColor = aerialColor;
			//finalColor = aerialColor + (originalColor * (1.0 - aerialDensity));
		}

		

		/*clipSpace = vec3(pixelPosition * vec2(2.0) - vec2(1.0), depth);
		hPos = inverse(variables.viewMatrix) * vec4(clipSpace, 1.0);
		float realDepth = length(hPos.xyz / hPos.w - variables.viewPosition);
		//float realDepth = GetDepth(depth);

		//vec4 screenPosition;
		//screenPosition.xy = (((gl_FragCoord.xy + 0.5) / (variables.resolution.xy)) - 0.5) * 2.0;
		//screenPosition.z = 0.0;
		//screenPosition.w = 1.0;
		//vec4 pixelWorldPosition = inverse(variables.viewMatrix) * screenPosition;
		//pixelWorldPosition /= pixelWorldPosition.w;
	
		float slice = realDepth * (1.0 / 4.0);
		float weight = 1.0;
		if (slice < 0.5)
		{
			weight = clamp(slice * 2.0, 0.0, 1.0);
			slice = 0.5;
		}
		float w = sqrt(slice / 32.0);
		//float slice = sqrt(1.0 - GetDepth(depth));
		//float w = sqrt(slice / 32.0);
		//float w = slice;
		vec4 aerialPerspective = weight * texture(aerialSampler, vec3(inCoordinates, w));

		finalColor = mix(finalColor, aerialPerspective.rgb, aerialPerspective.a);*/
	}

	outColor = vec4(finalColor, 1.0);
}