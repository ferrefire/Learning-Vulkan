#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

layout(set = 0, binding = 4) uniform sampler2D shadowLod0Sampler;
layout(set = 0, binding = 5) uniform sampler2D shadowLod1Sampler;

const vec3 lightColor = vec3(1);
//const vec3 lightDirection = vec3(0.25, 0.5, 0.25);

#include "depth.glsl"

vec3 DiffuseLighting(vec3 normal, vec3 color, float ambient)
{
	float diffuseStrength = max(dot(normal, variables.lightDirection), ambient);
	vec3 diffuse = color * diffuseStrength;

	return diffuse;
}

vec3 DiffuseLighting(vec3 normal, vec3 color)
{
	return DiffuseLighting(normal, color, 0.1);
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

vec3 GroundFog(vec3 color, float depth, float height)
{
	float heightFactor = pow(1.0 - (height / 5000.0), 8.0);
	return mix(color, vec3(1), depth + (depth * heightFactor));	
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
			vec2 coords = projectionCoordinates.xy + vec2(x, y) * texelSize;
			if (abs(coords.x - 0.5) > 0.5 || abs(coords.y - 0.5) > 0.5) continue;
			if (lod == 0) closestDepth = textureLod(shadowLod0Sampler, coords, 0).r;
			else if (lod == 1) closestDepth = textureLod(shadowLod1Sampler, coords, 0).r; 
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

float GetShadow(vec4 shadowSpace, int lod, int range)
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
	//else if (lod == 1) viewCoordinates = variables.shadowLod1Matrix * vec4(variables.viewPosition, 1.0);
	//viewCoordinates.xyz = viewCoordinates.xyz / viewCoordinates.w;

	//float xDis = abs(projectionCoordinates.x - viewCoordinates.x);
	//float yDis = abs(projectionCoordinates.y - viewCoordinates.y);
	//float dis = max(xDis, yDis) * 0.5;

	//float dis = distance(viewCoordinates.xy, projectionCoordinates.xy) * 0.5;
	
	projectionCoordinates.xy = projectionCoordinates.xy * 0.5 + 0.5;

	//int range = dis < (lod == 0 ? 0.25 : 0.125) ? 1 : 0;
	//if (lod == 1) range = 1;

	//range = 0;

	if (range < 0)
	{
		int newRange = abs(range);
		range = 0;
		if (projectionCoordinates.y > 0.5)
		{
			range = int(round(mix(0, newRange, (projectionCoordinates.y - 0.5) * 2.0)));
		}
	}
	vec2 blendResult = BlendShadow(projectionCoordinates, range, lod);
	float shadow = blendResult.x;

	if (variables.shadowBounding == 1 && shadow > 0.0 && lod == 0)
	{
		float blend = max(blendResult.y, dis);
		shadow = mix(shadow, 0.0, pow(blend, 4));
	}
	//else if (lod == 1 && blendResult.y > 0.5)
	//{
	//	shadow = mix(shadow, 0.0, (blendResult.y - 0.5) * 2.0);
	//}
	return (shadow);
}

#endif