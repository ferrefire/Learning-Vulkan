#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

layout(set = 0, binding = 4) uniform sampler2D shadowLod0Sampler;
layout(set = 0, binding = 5) uniform sampler2D shadowLod1Sampler;

const vec3 lightColor = vec3(1);
//const vec3 lightDirection = vec3(0.25, 0.5, 0.25);

#include "depth.glsl"

vec3 DiffuseLighting(vec3 normal, vec3 color)
{
	float diffuseStrength = max(dot(normal, variables.lightDirection), 0.1);
	vec3 diffuse = color * diffuseStrength;

	return diffuse;
}

vec3 SpecularLighting(vec3 normal, vec3 viewDirection, float shininess)
{
	vec3 halfwayDirection = normalize(variables.lightDirection + viewDirection);
	float specular = pow(max(dot(normal, halfwayDirection), 0.0), shininess);
	vec3 specularColor = lightColor * specular;

	return specularColor;
}

vec3 Fog(vec3 color, float depth)
{
	return mix(color, vec3(1), depth);	
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

vec2 BlendShadow(vec3 projectionCoordinates, int range, int lod)
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
			if (lod == 0) closestDepth = texture(shadowLod0Sampler, projectionCoordinates.xy + vec2(x, y) * texelSize).r;
			else if (lod == 1) closestDepth = texture(shadowLod1Sampler, projectionCoordinates.xy + vec2(x, y) * texelSize).r; 
	        //shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;

			//float diff = projectionCoordinates.z - closestDepth;
			//if (diff > 0.0) shadow += 1.0;
			if (x == 0 && y == 0) closest = closestDepth;
			shadow += (projectionCoordinates.z > closestDepth ? 1.0 : 0.0);
	    }
	}
	if (range > 0) shadow /= pow((range * 2) + 1, 2);

	return (vec2(shadow, closest));
}

float GetShadow(vec4 shadowSpace, int lod)
{
	vec3 projectionCoordinates = shadowSpace.xyz / shadowSpace.w;
	//if (lod == 0) projectionCoordinates = projectionCoordinates * 0.5 + 0.5;
	//else if (lod == 1) projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;
	//projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	if (abs(projectionCoordinates.x) > 1.0 || abs(projectionCoordinates.y) > 1.0 || abs(projectionCoordinates.z) > 1.0) return (0.0);

	//if (projectionCoordinates.z > 1.0 || projectionCoordinates.x > 1.0 || projectionCoordinates.x < 0.0 || 
	//	projectionCoordinates.y > 1.0 || projectionCoordinates.y < 0.0) return (0.0);

	float dis = max(abs(projectionCoordinates.x), abs(projectionCoordinates.y));
	projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;

	//float closestDepth;
	//if (lod == 0) closestDepth = texture(shadowLod0Sampler, projectionCoordinates.xy).r;
	//else if (lod == 1) closestDepth = texture(shadowLod1Sampler, projectionCoordinates.xy).r;
	//float shadow = projectionCoordinates.z > closestDepth ? 1.0 : 0.0;

	//float shadow = 1.0;
	int range = clamp((dis < 0.25 ? 1 : 0) - lod, 0, 2);
	vec2 blendResult = BlendShadow(projectionCoordinates, range, lod);
	float shadow = blendResult.x;
	//shadow = mix(0.3, 1.0, shadow);

	//float diff = currentDepth - closestDepth;

	//if (projectionCoordinates.z > closestDepth)
	if (shadow > 0.0)
	{
		float centerX = (projectionCoordinates.x - 0.5) * 2.0;
		centerX = abs(centerX);
		float centerY = (projectionCoordinates.y - 0.5) * 2.0;
		centerY = abs(centerY);
		float center = max(centerX, centerY);
		center = pow(center, 1.5);
		float blend = max(blendResult.y, center);
		shadow = mix(shadow, 0.0, pow(blend, 4));
	}
	return (shadow);
}

#endif