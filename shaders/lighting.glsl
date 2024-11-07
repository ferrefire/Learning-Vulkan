#ifndef LIGHTING_INCLUDED
#define LIGHTING_INCLUDED

layout(set = 0, binding = 4) uniform sampler2D shadowSampler;

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

float GetShadow(vec4 shadowSpace)
{
	vec3 projectionCoordinates = shadowSpace.xyz / shadowSpace.w;
	projectionCoordinates = projectionCoordinates * 0.5 + 0.5;

	if (projectionCoordinates.z > 1.0 || projectionCoordinates.x > 1.0 || projectionCoordinates.x < 0.0 || 
		projectionCoordinates.y > 1.0 || projectionCoordinates.y < 0.0) return (1.0);

	float closestDepth = textureLod(shadowSampler, projectionCoordinates.xy, 0).r;
	float currentDepth = projectionCoordinates.z;

	float diff = currentDepth - closestDepth;

	if (diff > 0.0)
	{
		float centerX = (projectionCoordinates.x - 0.5) * 2.0;
		centerX = abs(centerX);
		float centerY = (projectionCoordinates.y - 0.5) * 2.0;
		centerY = abs(centerY);
		float center = max(centerX, centerY);
		center = pow(center, 1.5);

		float blend = max(closestDepth, center);

		float shadow = mix(0.4, 1.0, pow(blend, 4));

		return (shadow);
	}
	else
	{
		return (1.0);
	}
}

#endif