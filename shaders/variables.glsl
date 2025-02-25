#ifndef VARIABLES_INCLUDED
#define VARIABLES_INCLUDED

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

#ifndef TERRAIN_CASCADE_COUNT
#define TERRAIN_CASCADE_COUNT 1
#endif

#ifndef SKY_COLOR
#define SKY_COLOR vec3(0.64, 0.886, 1.0)
#endif

layout(set = 0, binding = 0) uniform Variables 
{
	mat4 view;
	mat4 projection;
	mat4 viewMatrix;
	mat4 viewLodMatrix;
	mat4 shadowCascadeMatrix[CASCADE_COUNT];
	mat4 cullMatrix;
    float viewHeight;
    vec3 viewPosition;
    vec3 viewDirection;
    vec3 viewRight;
    vec3 viewUp;
	vec4 resolution;
	vec4 ranges;
	vec3 lightDirection;
	vec3 rotatedLightDirection;
	vec3 terrainOffset;
	vec2 terrainLod0Offset;
	vec2 terrainLod1Offset;
	float terrainTotalSize;
	float terrainTotalSizeMult;
	int terrainChunksLength;
	float terrainChunksLengthMult;
	float terrainHeight;
	float time;
	uint occlusionCulling;
	uint shadows;
	uint shadowBounding;
	uint shadowCascades;
	vec4 terrainShadowOffsets[TERRAIN_CASCADE_COUNT];
	vec4 terrainShadowDistances[TERRAIN_CASCADE_COUNT];
	//float terrainShadowDistance1;
} variables;

#endif