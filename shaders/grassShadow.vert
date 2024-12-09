#version 450

#extension GL_ARB_shading_language_include : require

struct GrassData
{
	uint posxz;
	uint normxz;
	uint posynormy;
	uint rot;
	uint scaxy;
	uint coly;
};

layout(std430, set = 1, binding = 0) buffer DataBuffer
{
	GrassData data[];
};

//layout(std430, set = 1, binding = 1) buffer LodDataBuffer
//{
//	GrassData lodData[];
//};

layout(set = 1, binding = 1) uniform GrassVariables
{
	uint grassBase;
	float grassBaseMult;
	uint grassCount;
	uint grassLodBase;
	float grassLodBaseMult;
	uint grassLodCount;
	uint grassTotalBase;
	float grassTotalBaseMult;
	uint grassTotalCount;
	float spacing;
	float spacingMult;
	float windStrength;
	float windFrequency;
} grassVariables;

layout(push_constant, std430) uniform PushConstants
{
    uint grassLod;
	uint shadowCascade;
} pc;

layout(location = 0) in vec3 inPosition;

//layout(location = 0) out vec3 worldPosition;
//layout(location = 1) out vec3 objectNormal;
//layout(location = 2) out vec3 terrainNormal;
//layout(location = 3) out vec3 grassColor;
//layout(location = 4) out vec2 uv;

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

float random(vec2 st)
{
    return fract(sin(dot(st.xy * 0.001, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
	//vec3 normal = normalize(mix(vec3(0, 0, -1), vec3(sign(inPosition.x) * 0.5, 0, 0), clamp(abs(inPosition.x) * 10, 0.0, 1.0)));

	vec3 position = vec3(0);
	vec3 rotation = vec3(0);
	//vec3 terrainNormal = vec3(0);
	vec2 clumpScale = vec2(1);
	//float colorVal = 1;

	//if (pc.grassLod == 0)
	//{
	//	position.xz = unpackHalf2x16(data[gl_InstanceIndex].posxz);
	//	//terrainNormal.xz = unpackHalf2x16(data[gl_InstanceIndex].normxz);
	//	vec2 yy = unpackHalf2x16(data[gl_InstanceIndex].posynormy);
	//	position.y = yy.x;
	//	//terrainNormal.y = yy.y;
	//	rotation.xy = unpackHalf2x16(data[gl_InstanceIndex].rot);
	//	clumpScale = unpackHalf2x16(data[gl_InstanceIndex].scaxy);
	//	//colorVal = unpackHalf2x16(data[gl_InstanceIndex].coly).y;
	//}
	//else if (pc.grassLod == 1)
	//{
	//	//position.xz = unpackHalf2x16(lodData[gl_InstanceIndex].posxz);
	//	//terrainNormal.xz = unpackHalf2x16(lodData[gl_InstanceIndex].normxz);
	//	//vec2 yy = unpackHalf2x16(lodData[gl_InstanceIndex].posynormy);
	//	//position.y = yy.x;
	//	//terrainNormal.y = yy.y;
	//	//rotation.xy = unpackHalf2x16(lodData[gl_InstanceIndex].rot);
	//	//vec2 xx = unpackHalf2x16(lodData[gl_InstanceIndex].scaxcoly);
	//	//clumpScale = xx.x;
	//	//colorVal = xx.y;
	//	uint dataIndex = gl_InstanceIndex + grassVariables.grassCount;
	//	position.xz = unpackHalf2x16(data[dataIndex].posxz);
	//	//terrainNormal.xz = unpackHalf2x16(data[dataIndex].normxz);
	//	vec2 yy = unpackHalf2x16(data[dataIndex].posynormy);
	//	position.y = yy.x;
	//	//terrainNormal.y = yy.y;
	//	rotation.xy = unpackHalf2x16(data[dataIndex].rot);
	//	clumpScale = unpackHalf2x16(data[dataIndex].scaxy);
	//	//colorVal = unpackHalf2x16(data[dataIndex].coly).y;
	//}

	uint dataIndex = gl_InstanceIndex;
	if (pc.grassLod == 1) dataIndex += grassVariables.grassCount;

	position.xz = unpackHalf2x16(data[dataIndex].posxz);
	vec2 yy = unpackHalf2x16(data[dataIndex].posynormy);
	position.y = yy.x;
	rotation.xy = unpackHalf2x16(data[dataIndex].rot);
	clumpScale = unpackHalf2x16(data[dataIndex].scaxy);

	position += variables.viewPosition;
	float ran = rotation.x;

	//float squaredDistance = SquaredDistance(position, variables.viewPosition);
	//float maxDistance = pow(grassVariables.grassTotalBase * grassVariables.spacing, 2);
	//float maxDistanceMult = pow(grassVariables.grassTotalBaseMult * grassVariables.spacingMult, 2);
	//float scale = clamp(squaredDistance, 0.0, maxDistance) * maxDistanceMult;
	//scale = 1.0 - pow(1.0 - scale, 4);
	//scale = 1.0 + scale * 2;
	////float scaleRan = (random(position.xz) * 0.2 - 0.1) + 0.5;
	//vec3 scaledPosition = inPosition * scale * 0.5;
	//scaledPosition.x *= 0.5 * scale;
	//scaledPosition.y *= pow(clumpScale.y + 0.25, 2);

	vec3 scaledPosition = inPosition;
	scaledPosition.x *= clumpScale.x;
	scaledPosition.y *= clumpScale.y;

	float angle = radians(ran * (inPosition.y + 0.25));
	scaledPosition = Rotate(scaledPosition, angle, vec3(1, 0, 0));
	//normal = Rotate(normal, angle, vec3(1, 0, 0));

	ran = rotation.y;

	angle = radians(ran);
	scaledPosition = Rotate(scaledPosition, angle, vec3(0, 1, 0));
	//normal = Rotate(normal, angle, vec3(0, 1, 0));

	//objectNormal = normal;

	vec3 worldPosition = ObjectToWorld(scaledPosition, mat4(1)) + position;

    //gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);
	//vec4 viewSpace = variables.viewMatrix * vec4(worldPosition, 1.0);
	//vec4 shadowSpace = variables.shadowLod0Matrix * vec4(worldPosition, 1.0);

    //gl_Position = variables.shadowLod0Matrix * vec4(worldPosition, 1.0);
	//gl_Position = variables.shadowLod1Matrix * vec4(worldPosition, 1.0);

	//grassColor = vec3(0.0916, 0.1, 0.0125) * (1.0 + (colorVal * 0.35 - 0.175));

	//uv = vec2(inPosition.x * 10 + 0.5, inPosition.y);

	gl_Position = variables.shadowCascadeMatrix[pc.shadowCascade] * vec4(worldPosition, 1.0);
}