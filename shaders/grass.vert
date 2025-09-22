#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

struct GrassData
{
	uint posxz;
	uint normxz;
	uint posynormy;
	uint rot;
	uint scaxy;
	uint colyscaz;
};

layout(std430, set = 1, binding = 0) readonly buffer DataBuffer
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
} pc;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inCoordinate;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 objectNormal;
layout(location = 2) out vec3 terrainNormal;
layout(location = 3) out vec3 grassColor;
layout(location = 4) out vec2 uv;
layout(location = 5) out vec4 shadowPositions[CASCADE_COUNT];
//layout(location = 5) out vec4 shadowLod0Position;
//layout(location = 6) out vec4 shadowLod1Position;
//layout(location = 7) out float ao;

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"
//#include "curve.glsl"

//const vec3 grassTint = DECODE_COLOR(vec3(94, 122, 48) / 255.0);
//const vec3 grassTint = DECODE_COLOR(vec3(106, 125, 49) / 255.0);
const vec3 grassTint = DECODE_COLOR(vec3(93, 99, 44) / 255.0);

float random(vec2 st)
{
    return fract(sin(dot(st.xy * 0.001, vec2(12.9898,78.233))) * 43758.5453123);
}

void main()
{
	//vec3 normal = normalize(mix(vec3(0, 0, -1), vec3(sign(inPosition.x) * 0.5, 0, 0), clamp(abs(inPosition.x) * 10.0, 0.0, 1.0)));
	vec3 normal = Rotate(vec3(0.0, 0.0, -1.0), radians(30.0) * ((inCoordinate.x - 0.5) * 2.0), vec3(0.0, 1.0, 0.0));
	//vec3 normal = vec3(0.0, 0.0, -1.0);

	vec3 position = vec3(0);
	vec3 rotation = vec3(0);
	terrainNormal = vec3(0);
	vec2 clumpScale = vec2(1);
	float colorVal = 1;

	uint dataIndex = gl_InstanceIndex;
	if (pc.grassLod == 1) dataIndex += grassVariables.grassCount;

	position.xz = unpackHalf2x16(data[dataIndex].posxz);
	terrainNormal.xz = unpackHalf2x16(data[dataIndex].normxz);
	vec2 yy = unpackHalf2x16(data[dataIndex].posynormy);
	position.y = yy.x;
	terrainNormal.y = yy.y;
	rotation.xy = unpackHalf2x16(data[dataIndex].rot);
	clumpScale = unpackHalf2x16(data[dataIndex].scaxy);
	colorVal = unpackHalf2x16(data[dataIndex].colyscaz).y;

	position += variables.viewPosition;
	float ran = rotation.x;

	/*float wave = rotation.x;
	vec3 scaledPosition = inPosition;

	float angle = radians(rotation.y);
	Curve3D c = Curve3D(vec3(0.0), vec3(0.0, 0.5 - wave * 0.25, 0.0), vec3(0.5, 1.0 - wave * 0.5, 0.0), vec3(1.0 + wave * 0.5, 1.0 - wave, 0.0));
	//use transformation matrix instead!!!!!!!!!!!!!!!!!!!!!
	c.p0 = Rotate(c.p0, angle, vec3(0, 1, 0));
	c.p1 = Rotate(c.p1, angle, vec3(0, 1, 0));
	c.p2 = Rotate(c.p2, angle, vec3(0, 1, 0));
	c.p3 = Rotate(c.p3, angle, vec3(0, 1, 0));

	float t = inPosition.y * clumpScale.y;
	vec3 curvePosition = CubicCurve3D(c, t);
	scaledPosition = curvePosition;
	vec3 curveDirection = normalize(CubicDerivative3D(c, t));
	vec3 curveNormal = normalize(Rotate(curveDirection, radians(-90.0), Rotate(vec3(0.0, 0.0, 1.0), angle, vec3(0, 1, 0))));
	vec3 curveSide = normalize(cross(curveDirection, curveNormal));
	scaledPosition += curveSide * inPosition.x * clumpScale.x;

	objectNormal = normalize(mix(-curveNormal, curveSide * sign(inPosition.x), abs((inPosition.x) * 10.0)));*/

	vec3 scaledPosition = inPosition;
	//scaledPosition.x *= clumpScale.x * 1.5;
	scaledPosition.x *= clumpScale.x;
	scaledPosition.y *= clumpScale.y;

	float angle = radians(ran * (inPosition.y + 0.25));
	scaledPosition = Rotate(scaledPosition, -angle, vec3(1, 0, 0));
	normal = Rotate(normal, -angle, vec3(1, 0, 0));

	angle = radians(rotation.y);
	scaledPosition = Rotate(scaledPosition, angle, vec3(0, 1, 0));
	normal = Rotate(normal, angle, vec3(0, 1, 0));

	objectNormal = normal;

	worldPosition = ObjectToWorld(scaledPosition, mat4(1)) + position;

	uv = vec2(inPosition.x * 10.0 + 0.5, inPosition.y);

	vec3 viewDirection = normalize(worldPosition - variables.viewPosition);
	float viewDotNormal = dot(normal, viewDirection);
	float thickenFactor = (1.0 - abs(viewDotNormal));
	worldPosition += thickenFactor * normal * 0.05;

	for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);

	grassColor = grassTint * (1.0 + (colorVal - 0.5));

	gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}