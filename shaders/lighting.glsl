#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

#ifndef TERRAIN_CASCADE_COUNT
#define TERRAIN_CASCADE_COUNT 1
#endif

#ifndef FOG_COLOR
#define FOG_COLOR vec3(0.64, 0.886, 1.0)
#endif

#ifndef LIGHT_COLOR
//#define LIGHT_COLOR vec3(1.0, 0.933, 0.89) * 1.5
#define LIGHT_COLOR DECODE_COLOR(vec3(255, 233, 201) / 255.0) * 2.0
#endif

layout(set = 0, binding = 5) uniform sampler2DShadow shadowSamplers[CASCADE_COUNT];
layout(set = 0, binding = 6) uniform sampler2D terrainShadowSamplers[TERRAIN_CASCADE_COUNT];
//layout(set = 0, binding = 7) uniform sampler2D skyViewSampler;

#ifndef VIEW_COMPUTE
layout(std430, set = 0, binding = 7) readonly buffer SunBuffer
{
	vec4 sunColor;
	vec4 skyColor;
};
#endif

//const vec3 lightColor = vec3(2);
//const vec3 lightColor = vec3(1.0, 0.933, 0.89) * 1.5;
//const vec3 lightDirection = vec3(0.25, 0.5, 0.25);

const float ambient = 0.1 * 0.5;
const float ambientMult = 2.0;
//const float ambient = 0.4;
const float shadowDis = 250.0;
const float shadowDepth = shadowDis / 25000.0;
const float shadowDepthMult = 1.0 / (shadowDis / 25000.0);
//const float rangeMults[] = {1.0 / 9.0, 1.0 / 25.0, 1.0 / 49.0};
const float rangeMults[] = {1.0 / 4.0, 1.0 / 8.0, 1.0 / 12.0};
const float texelSizes[] = {1.0 / 4096.0, 1.0 / 4096.0, 1.0 / 2048.0, 1.0 / 2048.0};
const float cascadeDistances[] = {50 / 25000.0, 175 / 25000.0, 500 / 25000.0, 900 / 25000.0};
//const int samples = 2;

#include "functions.glsl"
#include "depth.glsl"
//#include "variables.glsl"
//#include "heightmap.glsl"
//#include "atmosphere.glsl"

/*vec3 DiffuseLighting(vec3 normal, float shadow, float ao, float ao2, vec3 worldPos)
{
	if ((variables.lightDirection * 25000.0).y < worldPos.y)
		return (lightColor * ao2);

	float diffuseStrength = mix(max(dot(normal, variables.lightDirection), ao), ao2, shadow);
	vec3 diffuse = lightColor * diffuseStrength;

	return diffuse;
}*/

/*vec3 DiffuseLightingRealistic(vec3 normal, vec3 position, float shadow, float ao, float ao2)
{
	vec2 uv;
	vec3 rayDirection = normal;
	rayDirection = normalize(Rotate(rayDirection, radians(-90.0), vec3(1.0, 0.0, 0.0)));
	vec3 sunDirection = variables.rotatedLightDirection;
	vec3 rayStart = vec3(0.0, 0.0, PR + RADIUS_OFFSET + (position.y - variables.terrainOffset.y) * 0.001);
	float viewHeight = length(rayStart);
	//vec3 upVector = normalize(rayStart);
	//float viewAngle = acos(dot(sunDirection, upVector));
	float viewAngle = acos(0);
	float lightAngle = acos(dot(normalize(vec3(sunDirection.xy, 0.0)), normalize(vec3(sunDirection.xy, 0.0))));
	bool surfaceIntersect = SphereIntersectNearest(rayStart, sunDirection, PR) >= 0.0;
	uv = ViewUV(vec2(viewAngle, lightAngle), vec2(32, 32), viewHeight, surfaceIntersect);
	vec3 skyColor = texture(skyViewSampler, uv).rgb * LIGHT_COLOR * 4.0;
	float diffuseStrength = mix(max(dot(normal, variables.lightDirection), ao), ao2, shadow);
	//vec3 diffuseColor = clamp(skyColor * diffuseStrength, vec3(ao2), lightColor * 8.0);
	vec3 diffuse = skyColor * diffuseStrength;

	return (diffuse);
}

vec3 DiffuseLightingRealistic(vec3 normal, vec3 position, float shadow)
{
	return (DiffuseLightingRealistic(normal, position, shadow, ambient, ambient));
}*/

vec3 ToStandard(vec3 color)
{
	return (pow(color, vec3(1.0 / 2.2)));
}

vec3 ColorToStandard(vec3 color)
{
	return (pow(color / 255.0, vec3(1.0 / 2.2)));
}

vec3 DiffuseLighting(vec3 normal, float shadow, float ao, float ao2)
{
	//vec3 sunColor = textureLod(skyViewSampler, vec2(0.0, 0.49), 0.0).rgb * 2.0;

	float diffuseStrength = mix(max(dot(normal, variables.lightDirection), ao * ambientMult), ao2 * ambientMult, shadow);
	vec3 diffuse = sunColor.rgb * diffuseStrength;
	//vec3 diffuse = vec3(diffuseStrength);

	return diffuse;
}

vec3 DiffuseLighting(vec3 normal, float shadow, float ao)
{
	return DiffuseLighting(normal, shadow, ao, ao);
}

vec3 DiffuseLighting(vec3 normal, float shadow)
{
	return DiffuseLighting(normal, shadow, ambient);
}

vec3 DiffuseLighting(vec3 normal)
{
	return DiffuseLighting(normal, 0.0, ambient);
}

/*vec3 SpecularLightingRealistic(vec3 normal, vec3 viewDirection, float shininess, vec3 position)
{
	vec2 uv;
	vec3 rayDirection = normal;
	rayDirection = normalize(Rotate(rayDirection, radians(-90.0), vec3(1.0, 0.0, 0.0)));
	vec3 sunDirection = variables.rotatedLightDirection;
	vec3 rayStart = vec3(0.0, 0.0, PR + RADIUS_OFFSET + (position.y - variables.terrainOffset.y) * 0.001);
	float viewHeight = length(rayStart);
	//vec3 upVector = normalize(rayStart);
	//float viewAngle = acos(dot(sunDirection, upVector));
	float viewAngle = acos(0);
	float lightAngle = acos(dot(normalize(vec3(sunDirection.xy, 0.0)), normalize(vec3(sunDirection.xy, 0.0))));
	bool surfaceIntersect = SphereIntersectNearest(rayStart, sunDirection, PR) >= 0.0;
	uv = ViewUV(vec2(viewAngle, lightAngle), vec2(32, 32), viewHeight, surfaceIntersect);
	vec3 skyColor = texture(skyViewSampler, uv).rgb * LIGHT_COLOR * 4.0;

	vec3 halfwayDirection = normalize(variables.lightDirection + viewDirection);
	float specular = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
	vec3 specularColor = skyColor * specular;

	return specularColor;
}*/

vec3 SpecularLighting(vec3 normal, vec3 viewDirection, float shininess)
{
	vec3 halfwayDirection = normalize(variables.lightDirection + viewDirection);
	float specular = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
	vec3 specularColor = (sunColor.rgb * 1.0) * specular;

	return specularColor;
}

vec3 Fog(vec3 color, vec3 fogColor, float depth)
{
	return mix(color, fogColor, depth);	
}

vec3 Fog(vec3 color, float depth)
{
	return Fog(color, FOG_COLOR, depth);	
}

vec3 GroundFog(vec3 color, float depth, float height)
{
	return (Fog(color, depth));
	//float heightFactor = pow(1.0 - (height / 5000.0), 8.0);
	//return mix(color, vec3(1), depth + (depth * heightFactor));	
}

vec3 NormalPower(vec3 normal, float power)
{
	normal.xz *= power;
	return (normalize(normal));
}

vec3 NormalToTangent(vec3 normal)
{
    vec3 t1 = cross(normal, vec3(0, 1, 0));
    vec3 t2 = cross(normal, vec3(0, 0, 1));
    return ((dot(t1, t1) > dot(t2, t2)) ? t1 : t2);
}

float LightDot(vec3 normal)
{
	return max(dot(normal, variables.lightDirection), 0.0);
}

/*vec2 BlendShadow(vec3 projectionCoordinates, int range, int lod)
{
	float shadow = 0.0;
	float closest = 0;
	vec2 texelSize;
	if (lod == 0) texelSize = 1.0 / textureSize(shadowLod0Sampler, 0);
	else if (lod == 1) texelSize = 1.0 / textureSize(shadowLod1Sampler, 0);

	for(int x = -range; x <= range; ++x)
	{
	    for(int y = -range; y <= range; ++y)
	    {
	        float closestDepth = 0;
			vec2 coords = projectionCoordinates.xy + vec2(x, y) * texelSize;
			if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
			if (lod == 0) closestDepth = textureLod(shadowLod0Sampler, coords, 0).r;
			else if (lod == 1) closestDepth = textureLod(shadowLod1Sampler, coords, 0).r;
			if (x == 0 && y == 0) closest = closestDepth;
			shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
	    }
	}
	if (range > 0) shadow /= pow((range * 2) + 1, 2);

	return (vec2(shadow, closest));
}

float GetShadow(vec4 shadowSpace, int lod, int range, float rangeDis)
{
	vec3 projectionCoordinates = shadowSpace.xyz / shadowSpace.w;

	if (abs(projectionCoordinates.x) > 1.0 || abs(projectionCoordinates.y) > 1.0 || abs(projectionCoordinates.z) > 1.0) return (0.0);

	float dis = 0;
	if (variables.shadowBounding == 1)
	{
		vec4 viewCoordinates;
		if (lod == 0) viewCoordinates = variables.shadowLod0Matrix * vec4(variables.viewPosition, 1.0);
		else if (lod == 1) viewCoordinates = variables.shadowLod1Matrix * vec4(variables.viewPosition, 1.0);
		viewCoordinates.xyz = viewCoordinates.xyz / viewCoordinates.w;

		dis = distance(viewCoordinates.xy, projectionCoordinates.xy) * 0.5;

		range = (dis < 0.1 ? 1 : 0);
		//range = 0;
	}
	
	projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;

	if (range < 0)
	{
		int newRange = abs(range);
		range = 0;
		if (projectionCoordinates.y > rangeDis)
		{
			range = int(round(mix(0, newRange, clamp((projectionCoordinates.y - rangeDis) / (1.0 - rangeDis), 0.0, 1.0))));
		}
	}
	vec2 blendResult = BlendShadow(projectionCoordinates, range, lod);
	float shadow = blendResult.x;

	if (variables.shadowBounding == 1 && shadow > 0.0 && lod == 0)
	{
		float blend = max(blendResult.y, dis);
		shadow = mix(shadow, 0.0, pow(blend, 4));
	}
	return (shadow);
}

float GetShadow(vec4 shadowSpace, int lod, int range)
{
	return (GetShadow(shadowSpace, lod, range, 0.5));
}
*/

float BlendCascadedShadow(vec3 projectionCoordinates, int lod, int samples)
{
	float shadow = 0.0;
	//float closest = 0.0;
	float closestDepth = 0.0;
	vec2 coords = vec2(0);
	//vec2 texelSize = 1.0 / textureSize(shadowSamplers[lod], 0);
	//vec2 texelSize = vec2(texelSizes[lod]);
	vec2 texelSize = 1.0 / textureSize(shadowSamplers[lod], 0);
	//int samples = 3 - lod;
	//int samples = 1;

	/*for(int x = -range; x <= range; ++x)
	{
	    for(int y = -range; y <= range; ++y)
	    {
			//if (abs(x) + abs(y) > range) continue;

			//samples++;

			coords = projectionCoordinates.xy + vec2(x, y) * texelSize;

			if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;

			closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
			if (x == 0 && y == 0) closest = closestDepth;

			shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
	    }
	}*/

	//if (range == 0)
	//{
	//	coords = projectionCoordinates.xy;
	//	closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
	//	shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
	//	return (shadow);
	//}

	/*for (int x = 0; x <= 1; x++)
	{
		for (int y = 0; y <= 1; y++)
		{
			coords = projectionCoordinates.xy + (vec2(x, y) - 0.5) * texelSize;
			if (abs(coords.x - 0.5) < 0.5 && abs(coords.y - 0.5) < 0.5)
			{
				closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
				shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
			}

			if (x == 0)
			{
				coords = projectionCoordinates.xy + ((vec2(y, 0) - 0.5) * 3.0) * texelSize;
				if (abs(coords.x - 0.5) < 0.5 && abs(coords.y - 0.5) < 0.5)
				{
					closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
					shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
				}
			}
			else if (x == 1)
			{
				coords = projectionCoordinates.xy + ((vec2(0, y) - 0.5) * 3.0) * texelSize;
				if (abs(coords.x - 0.5) < 0.5 && abs(coords.y - 0.5) < 0.5)
				{
					closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
					shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
				}
			}

			coords = projectionCoordinates.xy + ((vec2(x, y) - 0.5) * 3.0) * texelSize;
			if (abs(coords.x - 0.5) < 0.5 && abs(coords.y - 0.5) < 0.5)
			{
				closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
				shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
			}
		}
	}*/

	//if (samples <= 0)
	//{
	//	closestDepth = textureLod(shadowSamplers[lod], projectionCoordinates.xy, 0).r;
	//	shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
	//	return (shadow);
	//}

	//closestDepth = textureLod(shadowSamplers[lod], projectionCoordinates.xy, 0).r;
	//shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
	//if (samples <= 0) return (shadow);

	if (samples <= 0)
	{
		shadow = textureLod(shadowSamplers[lod], projectionCoordinates.xyz, 0);
		return (shadow);
	}
	else if (samples <= 1)
	{
		for (int x = 0; x <= 1; x++)
		{
			for (int y = 0; y <= 1; y++)
			{
				coords = projectionCoordinates.xy + (vec2(x, y) - 0.5) * texelSize;
				if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
				shadow += textureLod(shadowSamplers[lod], vec3(coords.xy, projectionCoordinates.z), 0);
			}
		}
		return (shadow * rangeMults[0]);
	}
	else if (samples <= 2)
	{
		for (int x = -1; x <= 1; x++)
		{
			for (int y = -1; y <= 1; y++)
			{
				coords = projectionCoordinates.xy + vec2(x, y) * texelSize;
				if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
				shadow += textureLod(shadowSamplers[lod], vec3(coords.xy, projectionCoordinates.z), 0);
			}
		}
		return (shadow * 0.11111111111111);
	}

	/*for (int x = 0; x <= 1; x++)
	{
		for (int y = 0; y <= 1; y++)
		{
			coords = projectionCoordinates.xy + (vec2(x, y) - 0.5) * texelSize;
			if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
			//closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
			//shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
			shadow += texture(shadowSamplers[lod], vec3(coords.xy, projectionCoordinates.z));
		}
	}

	for (int x = 0; x <= 1; x++)
	{
		coords = projectionCoordinates.xy + ((vec2(x, 0) - 0.5) * 3.0) * texelSize;
		if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
		//closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
		//shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
		shadow += texture(shadowSamplers[lod], vec3(coords.xy, projectionCoordinates.z));
	}

	for (int y = 0; y <= 1; y++)
	{
		coords = projectionCoordinates.xy + ((vec2(0, y) - 0.5) * 3.0) * texelSize;
		if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
		//closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
		//shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
		shadow += texture(shadowSamplers[lod], vec3(coords.xy, projectionCoordinates.z));
	}

	if (samples <= 2) return (shadow * rangeMults[1]);

	for (int x = 0; x <= 1; x++)
	{
		for (int y = 0; y <= 1; y++)
		{
			coords = projectionCoordinates.xy + ((vec2(x, y) - 0.5) * 3.0) * texelSize;
			if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
			//closestDepth = textureLod(shadowSamplers[lod], coords, 0).r;
			//shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
			shadow += texture(shadowSamplers[lod], vec3(coords.xy, projectionCoordinates.z));
		}
	}

	return (shadow * rangeMults[2]);*/

	return (0.0);
}

bool ValidProjection(vec3 projectionCoordinates)
{
	if (abs(projectionCoordinates.x - 0.5) > 0.5) return (false);
	if (abs(projectionCoordinates.y - 0.5) > 0.5) return (false);
	if (abs(projectionCoordinates.z - 0.5) > 0.5) return (false);
	return (true);
}

struct ShadowResults
{
	float shadow;
	int lod;
	float edgeBlend;
	float reduction;
};

float GetCascadedShadowLod(int i, vec4 shadowSpace, float depth)
{
	float shadow = 0.0;
	int range = 1;

	float depthDistance = depth * variables.ranges.y;
	if (depthDistance < 10.0) range = 2;

	vec3 projectionCoordinates = shadowSpace.xyz / shadowSpace.w;
	projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;

	if (ValidProjection(projectionCoordinates))
	{
		shadow = BlendCascadedShadow(projectionCoordinates, i, range);
	}
	
	return (shadow);
}

ShadowResults GetCascadedShadowResults(vec4 shadowSpaces[CASCADE_COUNT], float depth)
{
	#ifndef SHADOWS_ENABLED
	return (ShadowResults(0.0, -1, 0.0, 1.0));
	#endif

	vec3 projectionCoordinates;
	vec3 blendCoordinates;
	float shadow = 0.0;
	float blendShadow = 0.0;
	int lod = -1;
	int startLod = -1;
	int range = 1;
	float dis = depth * variables.ranges.y;

	if (dis < 10.0) range = 2;

	/*for (int i = 0; i < CASCADE_COUNT; i++)
	{
		if (dis < variables.shadowCascadeDistances[i].x)
		{
			lod = i;
			break;
		}
	}

	if (lod != -1)
	{
		projectionCoordinates = shadowSpaces[lod].xyz / shadowSpaces[lod].w;
		projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
		shadow = BlendCascadedShadow(projectionCoordinates, lod, range);
	}*/

	/*for (int i = 0; i < CASCADE_COUNT; i++)
	{
		projectionCoordinates = shadowSpaces[i].xyz / shadowSpaces[i].w;
		projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;

		if (ValidProjection(projectionCoordinates))
		{
			blendShadow = BlendCascadedShadow(projectionCoordinates, i, range);
			if (blendShadow > shadow)
			{
				lod = i;
				shadow = blendShadow;
				break;
			}
		}
	}

	if (lod >= 0)
	{
		//shadow = GetCascadedShadowLod(lod, shadowSpaces[lod], depth);
		if (lod + 1 < CASCADE_COUNT)
		{
			float edgeShadow = max(max(abs(projectionCoordinates.x - 0.5), abs(projectionCoordinates.y - 0.5)), abs(projectionCoordinates.z - 0.5)) - (0.5 - variables.shadowCascadeMergeStrength);
			if (edgeShadow >= 0.0)
			{
				blendShadow = GetCascadedShadowLod(lod + 1, shadowSpaces[lod + 1], depth);
				//if (blendShadow > shadow) shadow = blendShadow;
				edgeShadow /= variables.shadowCascadeMergeStrength;
				shadow *= 1.0 - edgeShadow;
				shadow += blendShadow * edgeShadow;
			}
		}
	}*/

	//for (int i = CASCADE_COUNT - 1; i >= 0; i--)
	//{
	//	//if (lod != -1) break;
	//	projectionCoordinates = shadowSpaces[i].xyz / shadowSpaces[i].w;
	//	projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
	//	if (abs(projectionCoordinates.x - 0.5) < 0.5 && abs(projectionCoordinates.y - 0.5) < 0.5) lod = i;
	//}
	//if (lod == -1) return (0.0);

	////if (range < 0)
	//{
	//	float depthDistance = depth * variables.ranges.y;
	//	if (depthDistance < 10.0) range = 2;
	//	else range = 1;
	//	//else if (depthDistance < 5000.0) range = 1;
	//	//else range = 0;
	//	//range = 0;
	//}
	
	//range = 0;
	//if (maxCascade < 0) maxCascade = CASCADE_COUNT;

	//int shadowLod = -1;
	//float edgeShadow = 1.0;
	//bool stopNext = false;

	for (int i = 0; i < CASCADE_COUNT; i++)
	{
		//if (startLod != -1 && i > startLod + 1) break;
		if (lod != -1 && i > lod + 1) break;

		projectionCoordinates = shadowSpaces[i].xyz / shadowSpaces[i].w;
		projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;

		if (ValidProjection(projectionCoordinates))
		{
			//if (abs(projectionCoordinates.y - 0.5) > 0.4 || abs(projectionCoordinates.x - 0.5) > 0.4) return (ShadowResults(1.0, -1, 0.0, 1.0));
			//else return (ShadowResults(0.0, -1, 0.0, 1.0));

			//blendShadow = BlendCascadedShadow(projectionCoordinates, i, range);
			//shadow = BlendCascadedShadow(projectionCoordinates, i, range);
			blendShadow = BlendCascadedShadow(projectionCoordinates, i, range);
			//shadow = clamp(shadow + blendShadow, 0.0, 1.0);
			if (blendShadow > shadow)
			{
				shadow = blendShadow;
				if (lod == -1)
				{
					lod = i;
				}
				//lod = i;
				//if (startLod == -1) startLod = lod;
				//shadow = blendShadow;
				//break;
			}

			//break;
			if (max(max(abs(projectionCoordinates.x - 0.5), abs(projectionCoordinates.y - 0.5)), abs(projectionCoordinates.z - 0.5)) < (0.5 - variables.shadowCascadeMergeStrength)) break;
		}
	}

	//float edgeBlend = 0.0;
	//if (shadowLod >= CASCADE_COUNT - 1) edgeBlend = max(max(abs(projectionCoordinates.x - 0.5), abs(projectionCoordinates.y - 0.5)), projectionCoordinates.z - 0.75);
	//else if (shadowLod >= CASCADE_COUNT - 2) edgeBlend = abs(projectionCoordinates.x - 0.5);
	//if (shadowLod != -1) edgeBlend = abs(projectionCoordinates.x - 0.5);

	ShadowResults result;
	result.reduction = 1.0;
	float edgeBlend = 0.0;
	//projectionCoordinates = shadowSpaces[CASCADE_COUNT - 1].xyz / shadowSpaces[CASCADE_COUNT - 1].w;
	//projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
	////projectionCoordinates = projectionCoordinates * 0.5 + 0.5;
	////edgeBlend = clamp(max(max(abs(projectionCoordinates.x - 0.5), projectionCoordinates.y - 0.25), projectionCoordinates.z - 0.5), 0.0, 0.5);
	//edgeBlend = max(max(abs(projectionCoordinates.x - 0.5), abs(projectionCoordinates.y - 0.5)), abs(projectionCoordinates.z - 0.5));
	//edgeBlend = max(max(abs(projectionCoordinates.x - 0.5), projectionCoordinates.y - 0.5), abs(projectionCoordinates.z - 0.5));
	//edgeBlend = max(abs(projectionCoordinates.x - 0.5), abs(projectionCoordinates.y - 0.5));
	//edgeBlend = clamp(max(abs(projectionCoordinates.x - 0.5), projectionCoordinates.y - 0.25), 0.0, 0.5);

	//commented this out
	//if (lod == -1 || lod == (CASCADE_COUNT - 1))
	//{
	//	edgeBlend = max(max(abs(projectionCoordinates.x - 0.5), abs(projectionCoordinates.y - 0.5)), abs(projectionCoordinates.z - 0.5));
	//	if (edgeBlend >= 0.4) result.reduction = clamp(1.0 - (edgeBlend - 0.4) * 10.0, 0.0, 1.0);
	//	if (edgeBlend <= 0.5) shadow *= result.reduction;
	//}
	////if (lod > (CASCADE_COUNT - 2) && edgeBlend <= 0.5) shadow *= result.reduction;

	result.shadow = shadow;
	result.lod = lod;
	result.edgeBlend = edgeBlend;

	return (result);

	/*projectionCoordinates = shadowSpaces[0].xyz / shadowSpaces[0].w;
	projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
	if (abs(projectionCoordinates.x - 0.5) <= 0.5 && abs(projectionCoordinates.y - 0.5) <= 0.5 && abs(projectionCoordinates.z - 0.5) <= 0.5)
	{
		shadow = BlendCascadedShadow(projectionCoordinates, 0, range);
	}

	if (CASCADE_COUNT <= 1) return (shadow);

	//if (CASCADE_COUNT > 1 && shadow == 0.0 && (force || (abs(projectionCoordinates.x - 0.5) >= 0.49 || abs(projectionCoordinates.y - 0.5) >= 0.49)))
	if (shadow == 0.0 && (abs(projectionCoordinates.x - 0.5) >= 0.495 || abs(projectionCoordinates.y - 0.5) >= 0.495 || abs(projectionCoordinates.z - 0.5) >= 0.495))
	{
		projectionCoordinates = shadowSpaces[1].xyz / shadowSpaces[1].w;
		projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
		if (abs(projectionCoordinates.x - 0.5) <= 0.5 && abs(projectionCoordinates.y - 0.5) <= 0.5 && abs(projectionCoordinates.z - 0.5) <= 0.5)
		{
			blendShadow = BlendCascadedShadow(projectionCoordinates, 1, range);
			if (blendShadow > 0.0) shadow = blendShadow;
		}
	}

	if (CASCADE_COUNT <= 2) return (shadow);

	if (shadow == 0.0 && (abs(projectionCoordinates.x - 0.5) >= 0.495 || abs(projectionCoordinates.y - 0.5) >= 0.495 || abs(projectionCoordinates.z - 0.5) >= 0.495))
	{
		projectionCoordinates = shadowSpaces[2].xyz / shadowSpaces[2].w;
		projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
		if (abs(projectionCoordinates.x - 0.5) <= 0.5 && abs(projectionCoordinates.y - 0.5) <= 0.5 && abs(projectionCoordinates.z - 0.5) <= 0.5)
		{
			blendShadow = BlendCascadedShadow(projectionCoordinates, 2, range);
			if (blendShadow > 0.0) shadow = blendShadow;
		}
	}

	if (CASCADE_COUNT <= 3) return (shadow);

	if (shadow == 0.0 && (abs(projectionCoordinates.x - 0.5) >= 0.495 || abs(projectionCoordinates.y - 0.5) >= 0.495 || abs(projectionCoordinates.z - 0.5) >= 0.495))
	{
		projectionCoordinates = shadowSpaces[3].xyz / shadowSpaces[3].w;
		projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
		if (abs(projectionCoordinates.x - 0.5) <= 0.5 && abs(projectionCoordinates.y - 0.5) <= 0.5 && abs(projectionCoordinates.z - 0.5) <= 0.5)
		{
			blendShadow = BlendCascadedShadow(projectionCoordinates, 3, range);
			if (blendShadow > 0.0) shadow = blendShadow;
		}
	}

	if (CASCADE_COUNT <= 4) return (shadow);

	if (shadow == 0.0 && (abs(projectionCoordinates.x - 0.5) >= 0.495 || abs(projectionCoordinates.y - 0.5) >= 0.495 || abs(projectionCoordinates.z - 0.5) >= 0.495))
	{
		projectionCoordinates = shadowSpaces[4].xyz / shadowSpaces[4].w;
		projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
		if (abs(projectionCoordinates.x - 0.5) <= 0.5 && abs(projectionCoordinates.y - 0.5) <= 0.5 && abs(projectionCoordinates.z - 0.5) <= 0.5)
		{
			blendShadow = BlendCascadedShadow(projectionCoordinates, 4, range);
			if (blendShadow > 0.0) shadow = blendShadow;
		}
	}*/

	//return (shadow);
}

float GetCascadedShadow(vec4 shadowSpaces[CASCADE_COUNT], float depth)
{
	return (GetCascadedShadowResults(shadowSpaces, depth).shadow);
}

//float GetCascadedShadow(vec4 shadowSpaces[CASCADE_COUNT], float depth)
//{
//	return (GetCascadedShadow(shadowSpaces, depth, ivec2(0, CASCADE_COUNT)));
//}

//float GetCascadedShadow(vec4 shadowSpaces[CASCADE_COUNT], float depth)
//{
//	return (GetCascadedShadow(shadowSpaces, depth, -1, -1));
//}
//
//float GetCascadedShadow(vec4 shadowSpace, int lod)
//{
//	return (GetCascadedShadow(shadowSpace, lod, 0, 2.0));
//}

float GetTerrainShadow(vec2 worldPosition)
{
	#ifndef TERRAIN_SHADOWS_ENABLED
	return (0.0);
	#endif

	vec2 uv;
	float shadow = 0.0;

	for (int i = TERRAIN_CASCADE_COUNT - 1; i >= 0; i--)
	{
		uv = (worldPosition - variables.terrainShadowOffsets[i].xy) / variables.terrainShadowDistances[i].x;

		if (abs(uv.x) < 0.5 && abs(uv.y) < 0.5)
		{
			float terrainShadow = 1.0 - textureLod(terrainShadowSamplers[i], uv + 0.5, 0).r;
			//if (terrainShadow > 0) shadow = terrainShadow;
			shadow += terrainShadow;
			if (shadow >= 1.0) return (1.0);
			//return (terrainShadow);
		}
	}

	return (shadow);
}

vec3 FinalLighting(vec3 color, vec3 diffuse)
{
	vec3 result = DECODE_COLOR(color * diffuse);

	return (result);
}

#endif