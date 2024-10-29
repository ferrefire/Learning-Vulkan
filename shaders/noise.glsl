#ifndef NOISE_INCLUDED
#define NOISE_INCLUDED

//uniform int noiseLayers;
//uniform float noiseScale;
//uniform float noiseSampleDistance;
//uniform float noiseSampleDistanceMult;

const int noiseLayers = 8;
const float noiseScale = 0.75;
const float noiseSampleDistance = 0.0001;
const float noiseSampleDistanceMult = 1666.666;

float blendDistance = 1.0;

vec3 mod289(vec3 x)
{
    return x - floor(x / 289.0) * 289.0;
}

vec2 mod289(vec2 x)
{
    return x - floor(x / 289.0) * 289.0;
}

vec3 permute(vec3 x)
{
    return mod289((x * 34.0 + 1.0) * x);
}

vec3 taylorInvSqrt(vec3 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec2 v)
{
    const vec4 C = vec4( 0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v -   i + dot(i, C.xx);

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

float GenerateNoise(vec2 uv, int layers)
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
		float left = snoise((uv + vec2(-noiseSampleDistance, 0)) * noiseScale * scale);
        float right = snoise((uv + vec2(noiseSampleDistance, 0)) * noiseScale * scale);
        float down = snoise((uv + vec2(0, -noiseSampleDistance)) * noiseScale * scale);
        float up = snoise((uv + vec2(0, noiseSampleDistance)) * noiseScale * scale);
		vec3 derivative = vec3((left - right) / (noiseSampleDistance * 2), 1.0, (down - up) / (noiseSampleDistance * 2));
		derivative = normalize(derivative);
		float steepness = 1.0 - (dot(derivative, vec3(0, 1, 0)) * 0.5 + 0.5);
		erosion += steepness * erosionWeight;
        float erodeSum = 1.0 / (1.0 + erosion * 1.5);
		noise += center * weight * erodeSum;

		//float center = snoise(uv * noiseScale * scale);
        //float right = snoise((uv + vec2(noiseSampleDistance, 0)) * noiseScale * scale);
        //float up = snoise((uv + vec2(0, noiseSampleDistance)) * noiseScale * scale);
		//vec3 derivative = vec3((right - center) / (noiseSampleDistance), 1.0, (up - center) / (noiseSampleDistance));
		//derivative = normalize(derivative);
		//float steepness = 1.0 - (dot(derivative, vec3(0, 1, 0)) * 0.5 + 0.5);
		//erosion += steepness * erosionWeight;
        //float erodeSum = 1.0 / (1.0 + erosion * 1.5);
        //noise += center * weight * erodeSum;

        maxNoise += weight;
        weight *= 0.375;
		erosionWeight *= 0.70;
        scale *= 2.5;
    }

	//noise = pow(noise, 0.5);
	//maxNoise = pow(maxNoise, 0.5);
    noise = InvLerp(0.0, maxNoise, noise);
    //noise = noise * noise;
	
	//float biome = max(abs(uv.x), abs(uv.y)) - 2.0;
	//if (biome >= -blendDistance && biome <= blendDistance)
	//{
	//	noise = mix(CubicCurve(LOWLANDS, noise), noise, (biome / blendDistance) * 0.5 + 0.5);
	//}
    //else if (biome <= -blendDistance)
    //{
    //    noise = CubicCurve(LOWLANDS, noise);
    //}

    return (noise);
}

#endif