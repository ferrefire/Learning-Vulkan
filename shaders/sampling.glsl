#ifndef SAMPLING_INCLUDED
#define SAMPLING_INCLUDED

#include "functions.glsl"

vec3 UnpackNormal(vec4 packedNormal, float scale)
{
	vec3 normal;

	normal = (packedNormal * 2.0 - 1.0).xyz;
	normal.xy *= scale;

	return (normalize(normal));
}

vec3 SampleTriplanarColor(sampler2D textureSampler, vec3 uv, vec3 weights)
{
	vec3 xz = (texture(textureSampler, uv.xz).rgb);
	vec3 xy = (texture(textureSampler, uv.xy).rgb);
	vec3 zy = (texture(textureSampler, uv.zy).rgb);
	vec3 result = xz * weights.y + xy * weights.z + zy * weights.x;

	return (result);
}

vec3 SampleTriplanarNormal(sampler2D textureSampler, vec3 uv, vec3 weights, vec3 normal, float power)
{
	vec2 uvX = uv.zy;
	vec2 uvY = uv.xz;
	vec2 uvZ = uv.xy;

	vec3 tangentX = UnpackNormal(texture(textureSampler, uvX), power);
	vec3 tangentY = UnpackNormal(texture(textureSampler, uvY), power);
	vec3 tangentZ = UnpackNormal(texture(textureSampler, uvZ), power);

	tangentX = vec3(tangentX.xy + normal.zy, abs(tangentX.z) * normal.x);
	tangentY = vec3(tangentY.xy + normal.xz, abs(tangentY.z) * normal.y);
	tangentZ = vec3(tangentZ.xy + normal.xy, abs(tangentZ.z) * normal.z);

	vec3 result = normalize(tangentX.zyx * weights.x + tangentY.xzy * weights.y + tangentZ.xyz * weights.z);

	return (result);
}

vec3 GetWeights(vec3 normal, float strength)
{
    vec3 weights = abs(normal);
	weights = NormalizeSum(weights);
	
	if (strength != 1.0)
	{
		weights = pow(weights, vec3(strength));
		weights = NormalizeSum(weights);
	}

	return (weights);
}

#endif