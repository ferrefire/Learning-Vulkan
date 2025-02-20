#ifndef HEIGHTMAP_INCLUDED
#define HEIGHTMAP_INCLUDED

//layout(set = 1, binding = 1) uniform sampler2DArray heightMapArraySampler;
//layout(set = 1, binding = 2) uniform sampler2D heightMapLod0Sampler;
//layout(set = 1, binding = 3) uniform sampler2D heightMapLod1Sampler;

//layout(set = 1, binding = 4) uniform HeightMapVariables
//{
//	vec2 terrainOffset;
//	vec2 terrainLod0Offset;
//	vec2 terrainLod1Offset;
//	float terrainTotalSize;
//	float terrainTotalSizeMult;
//	int terrainChunksLength;
//	float terrainChunksLengthMult;
//} heightMapVariables;

layout(set = 0, binding = 1) uniform sampler2DArray heightMapArraySampler;
layout(set = 0, binding = 2) uniform sampler2D heightMapLod0Sampler;
layout(set = 0, binding = 3) uniform sampler2D heightMapLod1Sampler;

const float worldSampleDistance = 1;
const float worldSampleDistanceMult = 0.0002;

//const float terrainTotalSize = 50000;
//const float terrainTotalSizeMult = 1.0 / 50000.0;

const float terrainChunkSize = 10000;
const float terrainChunkSizeMult = 0.0001;

const float terrainLod0Size = 2500;
const float terrainLod0SizeMult = 1.0 / terrainLod0Size;
const float terrainLod1Size = 5000;
const float terrainLod1SizeMult = 1.0 / terrainLod1Size;

//const int chunksLength = 5;
//const float chunksLengthMult = 1.0 / 5.0;
//const int chunksLength = 1;
//const float chunksLengthMult = 1.0;

struct Normals
{
	vec3 normalWS;
	vec3 normalTS;
};

float SampleArray(vec2 uvPosition)
{
	vec2 chunkUV = vec2(ceil(uvPosition.x * variables.terrainChunksLength), ceil(uvPosition.y * variables.terrainChunksLength));
	chunkUV = chunkUV * variables.terrainChunksLengthMult - variables.terrainChunksLengthMult * 0.5;

	vec2 indexUV = vec2(floor(chunkUV.x * variables.terrainChunksLength), floor(chunkUV.y * variables.terrainChunksLength));

	uvPosition -= chunkUV;
	uvPosition = (uvPosition * variables.terrainChunksLength * 2.0) * 0.5 + 0.5; //+ uv offset

	return textureLod(heightMapArraySampler, vec3(uvPosition, indexUV.x * variables.terrainChunksLength + indexUV.y), 0).r;
	
	//return textureLod(heightMapSampler, uvPosition, 0).r;
}

float SampleDynamic(vec2 worldPosition)
{
	vec2 worldUV = (worldPosition + variables.terrainOffset.xz) * variables.terrainTotalSizeMult;
	if (abs(worldUV.x) > 0.5 || abs(worldUV.y) > 0.5) return 0;
	if (abs(worldPosition.x - variables.terrainLod0Offset.x) < terrainLod0Size * 0.5 && abs(worldPosition.y - variables.terrainLod0Offset.y) < terrainLod0Size * 0.5)
	{
		return textureLod(heightMapLod0Sampler, (worldPosition - variables.terrainLod0Offset) * terrainLod0SizeMult + 0.5, 0).r;
	}
	else if (abs(worldPosition.x - variables.terrainLod1Offset.x) < terrainLod1Size * 0.5 && abs(worldPosition.y - variables.terrainLod1Offset.y) < terrainLod1Size * 0.5)
	{
		return textureLod(heightMapLod1Sampler, (worldPosition - variables.terrainLod1Offset) * terrainLod1SizeMult + 0.5, 0).r;
	}
	else
	{
		return (SampleArray(worldUV + 0.5));
	}
}

float SampleArrayWorld(vec2 worldPosition)
{
	vec2 worldUV = worldPosition * variables.terrainTotalSizeMult;
	if (abs(worldUV.x) > 0.5 || abs(worldUV.y) > 0.5) return 0;
	return (SampleArray(worldUV + 0.5));
}

vec3 SampleNormalDynamic(vec2 worldPosition, float power)
{
	float left = SampleDynamic(worldPosition - vec2(worldSampleDistance, 0));
    float right = SampleDynamic(worldPosition + vec2(worldSampleDistance, 0));
    float down = SampleDynamic(worldPosition - vec2(0, worldSampleDistance));
    float up = SampleDynamic(worldPosition + vec2(0, worldSampleDistance));
    vec3 normalTS = vec3((left - right) / worldSampleDistanceMult, 1, (down - up) / worldSampleDistanceMult);

	if (power == 1) return normalize(normalTS);

    normalTS.xz *= power;
    return (normalize(normalTS));
}

Normals SampleNormalsDynamic(vec2 worldPosition, float power)
{
	Normals normals;

	float left = SampleDynamic(worldPosition - vec2(worldSampleDistance, 0)) * variables.terrainHeight;
    float right = SampleDynamic(worldPosition + vec2(worldSampleDistance, 0)) * variables.terrainHeight;
    float down = SampleDynamic(worldPosition - vec2(0, worldSampleDistance)) * variables.terrainHeight;
    float up = SampleDynamic(worldPosition + vec2(0, worldSampleDistance)) * variables.terrainHeight;

    normals.normalWS = normalize(vec3(-(right - left), 1.0, -(up - down)));
    //normals.normalTS = normalize(vec3(-(right - left), power, -(up - down)));
    normals.normalTS = normalize(vec3(-(right - left) * power, 1.0, -(up - down) * power));
    //normals.normalTS = normalize(vec3(-(right - left), -(up - down), power));

    return (normals);
}

vec3 SampleNormalArray(vec2 worldPosition, float power)
{
	float left = SampleArrayWorld(worldPosition - vec2(worldSampleDistance, 0));
    float right = SampleArrayWorld(worldPosition + vec2(worldSampleDistance, 0));
    float down = SampleArrayWorld(worldPosition - vec2(0, worldSampleDistance));
    float up = SampleArrayWorld(worldPosition + vec2(0, worldSampleDistance));
    vec3 normalTS = vec3((left - right) / worldSampleDistanceMult, 1, (down - up) / worldSampleDistanceMult);

	if (power == 1) return normalize(normalTS);

    normalTS.xz *= power;
    return (normalize(normalTS));
}

float GetSteepness(vec3 normal)
{
    float steepness = dot(normal, vec3(0.0, 1.0, 0.0));
	steepness = (steepness + 1) * 0.5;
    //steepness = steepness * steepness;
    steepness = 1.0 - steepness;

    return steepness;
}

float GetTerrainHeight(vec2 worldPosition)
{
	float result = SampleDynamic(worldPosition);
	result *= variables.terrainHeight;
	result += variables.terrainOffset.y;

	return (result);
}

#endif