#ifndef NOISE_INCLUDED
#define NOISE_INCLUDED

//uniform int noiseLayers;
//uniform float noiseScale;
//uniform float noiseSampleDistance;
//uniform float noiseSampleDistanceMult;

//#include "perlinNoise.glsl"
#include "curve.glsl"

//precision highp float;
//precision highp vec2;
//precision highp vec3;
//precision highp vec4;

const int noiseLayers = 8;
const float noiseScale = 0.75;
const float noiseSampleDistance = 0.0001;
//const float noiseSampleDistanceMult = 1666.666;

float blendDistance = 1.0;

vec3 mod289(vec3 x)
{
    return (x - floor(x / 289.0) * 289.0);
}

vec2 mod289(vec2 x)
{
    return (x - floor(x / 289.0) * 289.0);
}

vec3 permute(vec3 x)
{
    return mod289((x * 34.0 + 1.0) * x);
}

vec3 taylorInvSqrt(vec3 r)
{
    return (1.79284291400159 - 0.85373472095314 * r);
}

float snoise(vec2 v)
{
    const vec4 C = vec4( 0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    vec2 i1;
    i1.x = step(x0.y, x0.x);
    i1.y = 1.0 - i1.x;

    vec2 x1 = x0 + C.xx - i1;
    vec2 x2 = x0 + C.zz;

    i = mod289(i);
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));

    vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x1, x1), dot(x2, x2)), 0.0);
    m = m * m;
    m = m * m;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= taylorInvSqrt(a0 * a0 + h * h);

    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.y = a0.y * x1.x + h.y * x1.y;
    g.z = a0.z * x2.x + h.z * x2.y;
    return (130.0 * dot(m, g)) * 0.5 + 0.5;
}

float InvLerp(float a, float b, float v)
{
    return (v - a) / (b - a);
}

/*float GenerateNoise(vec2 uv, int layers, int curve)
{
	//float noise = float(perlin(abs(uv.x), abs(uv.y), 0.0));
	//float noise = snoise(uv);
	float noise = abs(uv.y) - floor(abs(uv.y));
	return (noise);
}*/

float GenerateNoise(vec2 uv, int layers, int curve)
{
    float noise = 0;
    float maxNoise = 0;
    float weight = 1;
    float erosionWeight = 1;
    float scale = 1;
	float erosion = 0;

    for (int i = 0; i < layers; i++)
    {
        float center = snoise(uv * noiseScale * scale);
        float erodeSum = 1.0;

		if (layers > 1)
        {
            float left = snoise((uv + vec2(-noiseSampleDistance, 0)) * noiseScale * scale);
            float right = snoise((uv + vec2(noiseSampleDistance, 0)) * noiseScale * scale);
            float down = snoise((uv + vec2(0, -noiseSampleDistance)) * noiseScale * scale);
            float up = snoise((uv + vec2(0, noiseSampleDistance)) * noiseScale * scale);
		    vec3 derivative = vec3((left - right) / (noiseSampleDistance * 2), 1.0, (down - up) / (noiseSampleDistance * 2));
		    derivative = normalize(derivative);
		    float steepness = 1.0 - (dot(derivative, vec3(0, 1, 0)) * 0.5 + 0.5);
		    erosion += steepness * erosionWeight;
            erodeSum = 1.0 / (1.0 + erosion * 1.5);
        }

		noise += center * weight * erodeSum;

        maxNoise += weight;
        weight *= 0.375;
		erosionWeight *= 0.70;
        scale *= 2.5;
    }

    if (layers <= 1) return (noise);

	//noise = pow(noise, 0.5);
	//maxNoise = pow(maxNoise, 0.5);
    noise = InvLerp(0.0, maxNoise, noise);
    //noise = noise * noise;
	
	if (curve == 0)
	{
		float biome = max(abs(uv.x), abs(uv.y)) - 2.0;
		if (biome >= -blendDistance && biome <= blendDistance)
		{
			noise = mix(CubicCurve(MIDLANDS, noise), noise, (biome / blendDistance) * 0.5 + 0.5);
		}
    	else if (biome <= -blendDistance)
    	{
    	    noise = CubicCurve(MIDLANDS, noise);
    	}
	}

    return (noise);
}

float GenerateNoise(vec2 uv, int layers)
{
	return (GenerateNoise(uv, layers, -1));
}

vec3 GetDerivative(vec2 uv, int layers, float sampleDis)
{
    float center = GenerateNoise(uv, layers);
	float left = GenerateNoise((uv + vec2(-sampleDis, 0)), layers);
    float right = GenerateNoise((uv + vec2(sampleDis, 0)), layers);
    float down = GenerateNoise((uv + vec2(0, -sampleDis)), layers);
    float up = GenerateNoise((uv + vec2(0, sampleDis)), layers);
    
	vec3 derivative = vec3((left - right) / (sampleDis * 2), 0.0, (down - up) / (sampleDis * 2));
	derivative = normalize(derivative);

    return (derivative);
}

vec3 GetDerivative(vec2 uv, int layers)
{
    return (GetDerivative(uv, layers, noiseSampleDistance));
}

vec3 GetDerivative(vec2 uv)
{
    return (GetDerivative(uv, 1, noiseSampleDistance));
}

float GenerateSimpleNoise(vec2 uv, int layers, float persistance, float detail)
{
    float noise = 0;
    float maxNoise = 0;
    float weight = 1;
    float scale = 1;

    for (int i = 0; i < layers; i++)
    {
        float value = snoise(uv * noiseScale * scale);

		noise += value * weight;

        maxNoise += weight;
        weight *= persistance;
        scale *= detail;
    }

    noise = InvLerp(0.0, maxNoise, noise);

    return (noise);
}

vec3 GetSimpleDerivative(vec2 uv, int layers, float sampleDis, float persistance, float detail)
{
    float center = GenerateSimpleNoise(uv, layers, persistance, detail);
	float left = GenerateSimpleNoise((uv + vec2(-sampleDis, 0)), layers, persistance, detail);
    float right = GenerateSimpleNoise((uv + vec2(sampleDis, 0)), layers, persistance, detail);
    float down = GenerateSimpleNoise((uv + vec2(0, -sampleDis)), layers, persistance, detail);
    float up = GenerateSimpleNoise((uv + vec2(0, sampleDis)), layers, persistance, detail);
    
	vec3 derivative = vec3(-(right - left), 1.0, -(up - down));
	derivative = normalize(derivative);

    return (derivative);
}

#endif