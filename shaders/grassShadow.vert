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

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

void main()
{
	vec3 position = vec3(0);
	vec3 rotation = vec3(0);
	vec2 clumpScale = vec2(1);

	uint dataIndex = gl_InstanceIndex;
	if (pc.grassLod == 1) dataIndex += grassVariables.grassCount;

	position.xz = unpackHalf2x16(data[dataIndex].posxz);
	vec2 yy = unpackHalf2x16(data[dataIndex].posynormy);
	position.y = yy.x;
	rotation.xy = unpackHalf2x16(data[dataIndex].rot);
	clumpScale = unpackHalf2x16(data[dataIndex].scaxy);

	position += variables.viewPosition;
	float ran = rotation.x;

	vec3 scaledPosition = inPosition;
	scaledPosition.x *= clumpScale.x;
	scaledPosition.y *= clumpScale.y;

	float angle = radians(ran * (inPosition.y + 0.25));
	scaledPosition = Rotate(scaledPosition, angle, vec3(1, 0, 0));
	//normal = Rotate(normal, angle, vec3(1, 0, 0));

	ran = rotation.y;

	angle = radians(ran);
	scaledPosition = Rotate(scaledPosition, angle, vec3(0, 1, 0));

	vec3 worldPosition = ObjectToWorld(scaledPosition, mat4(1)) + position;

	gl_Position = variables.shadowCascadeMatrix[pc.shadowCascade] * vec4(worldPosition, 1.0);
}