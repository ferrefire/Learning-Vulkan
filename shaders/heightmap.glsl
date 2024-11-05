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
const float terrainLod0SizeMult = 0.0004;
const float terrainLod1Size = 5000;
const float terrainLod1SizeMult = 0.0002;

//const int chunksLength = 5;
//const float chunksLengthMult = 1.0 / 5.0;
//const int chunksLength = 1;
//const float chunksLengthMult = 1.0;

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
	//vec2 worldUV = (worldPosition + terrainWorldOffset) * terrainSizeMult;
	//vec2 worldUV = (worldPosition + variables.terrainOffset) * terrainChunkSizeMult;
	vec2 worldUV = (worldPosition + variables.terrainOffset) * variables.terrainTotalSizeMult;
	if (abs(worldUV.x) > 0.5 || abs(worldUV.y) > 0.5) return 0;
	if (abs(worldPosition.x - variables.terrainLod0Offset.x) < terrainLod0Size * 0.5 && abs(worldPosition.y - variables.terrainLod0Offset.y) < terrainLod0Size * 0.5)
	{
		//return (Sample((worldPosition - terrainOffsetLod0) * terrainLod0SizeMult + 0.5, 0));
		return textureLod(heightMapLod0Sampler, (worldPosition - variables.terrainLod0Offset) * terrainLod0SizeMult + 0.5, 0).r;
	}
	else if (abs(worldPosition.x - variables.terrainLod1Offset.x) < terrainLod1Size * 0.5 && abs(worldPosition.y - variables.terrainLod1Offset.y) < terrainLod1Size * 0.5)
	{
		//return (Sample((worldPosition - terrainOffsetLod1) * terrainLod1SizeMult + 0.5, 1));
		return textureLod(heightMapLod1Sampler, (worldPosition - variables.terrainLod1Offset) * terrainLod1SizeMult + 0.5, 0).r;
	}
	else
	{
		return (SampleArray(worldUV + 0.5));
	}

	//if (abs(worldPosition.x - terrainOffsetLod0.x) < terrainLod0Size * 0.5 && abs(worldPosition.y - terrainOffsetLod0.y) < terrainLod0Size * 0.5)
	//{
	//	return (Sample((worldPosition - terrainOffsetLod0) * terrainLod0SizeMult + 0.5, 0));
	//}
	//else if (abs(worldPosition.x - terrainOffsetLod1.x) < terrainLod1Size * 0.5 && abs(worldPosition.y - terrainOffsetLod1.y) < terrainLod1Size * 0.5)
	//{
	//	return (Sample((worldPosition - terrainOffsetLod1) * terrainLod1SizeMult + 0.5, 1));
	//}
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

float GetSteepness(vec3 normal)
{
    float steepness = dot(normal, vec3(0.0, 1.0, 0.0));
	steepness = (steepness + 1) * 0.5;
    //steepness = steepness * steepness;
    steepness = 1.0 - steepness;

    return steepness;
}

#endif