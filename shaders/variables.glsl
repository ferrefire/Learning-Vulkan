#ifndef VARIABLES_INCLUDED
#define VARIABLES_INCLUDED

layout(set = 0, binding = 0) uniform Variables 
{
	mat4 view;
	mat4 projection;
	mat4 viewMatrix;
	mat4 frustumMatrix;
	mat4 shadowLod0Matrix;
	mat4 shadowLod1View;
	mat4 shadowLod1Projection;
	mat4 shadowLod1Matrix;
	mat4 cullMatrix;
    vec3 viewPosition;
    vec3 viewDirection;
    vec3 viewRight;
    vec3 viewUp;
	vec4 resolution;
	vec3 lightDirection;
	vec2 terrainOffset;
	vec2 terrainLod0Offset;
	vec2 terrainLod1Offset;
	float terrainTotalSize;
	float terrainTotalSizeMult;
	int terrainChunksLength;
	float terrainChunksLengthMult;
	float time;
	uint occlusionCulling;
	uint shadows;
	vec2 frustumCorner1;
	vec2 frustumCorner2;
	vec2 frustumCorner3;
	vec2 frustumCorner4;
} variables;

#endif