#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

struct LeafData
{
	uint posxz;
	uint posyroty;
	uint scalxrotx;
	uint colxnormx;
	uint normyz;
	uint rotz;
};

layout(std430, set = 1, binding = 0) buffer DataBuffer
{
	LeafData data[];
};

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 localPosition;
layout(location = 2) out vec3 globalNormal;
layout(location = 3) out vec3 localNormal;
layout(location = 4) out vec3 leafColor;
layout(location = 5) out vec3 localCoordinates;
layout(location = 6) out vec4 shadowPositions[CASCADE_COUNT];

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

//const vec3 treeCenter = vec3(-3.05, 47.22, -5.725);
//const vec3 treeCenter = vec3(7.05, 47.22, -6.125);
const vec3 treeCenter = vec3(0.0, 40.0, 0.0);

void main()
{
	vec3 position = vec3(0);
	vec3 rotation = vec3(0);
	float scale = 1.0;
	float color = 1.0;

	uint dataIndex = gl_InstanceIndex;

	//position = data[dataIndex].position;

	position.xz = unpackHalf2x16(data[dataIndex].posxz);
	vec2 posyroty = unpackHalf2x16(data[dataIndex].posyroty);
	position.y = posyroty.x;
	rotation.y = posyroty.y * 360.0 - 180.0;
	vec2 scalxrotx = unpackHalf2x16(data[dataIndex].scalxrotx);
	scale = scalxrotx.x;
	rotation.x = scalxrotx.y * 360.0 - 180.0;
	vec2 colxnormx = unpackHalf2x16(data[dataIndex].colxnormx);
	color = colxnormx.x;
	vec2 normyz = unpackHalf2x16(data[dataIndex].normyz);
	vec3 normalPos = vec3(colxnormx.y, normyz.x, normyz.y);
	vec2 rotz = unpackHalf2x16(data[dataIndex].rotz);
	rotation.z = rotz.x * 360.0 - 180.0;

	position += variables.viewPosition;

	vec3 objectPosition = inPosition * scale;
	mat4 rotationMatrix = GetRotationMatrix(radians(rotation.y), vec3(0.0, 1.0, 0.0));
	rotationMatrix = rotationMatrix * GetRotationMatrix(radians(rotation.x * 0.5), vec3(1.0, 0.0, 0.0));
	rotationMatrix = rotationMatrix * GetRotationMatrix(radians(rotation.z * 0.25), vec3(0.0, 0.0, 1.0));
    objectPosition = (rotationMatrix * vec4(objectPosition, 1.0)).xyz;
	localNormal = (rotationMatrix * vec4(vec3(0, 1, 0), 1.0)).xyz;
	globalNormal = normalize((normalPos + objectPosition) - treeCenter);
	//normal = mix(localNormal, globalNormal, 0.5);

	localCoordinates = inPosition;
	localPosition = normalPos;
	worldPosition = ObjectToWorld(objectPosition, mat4(1)) + position;

	for (int i = 0; i < CASCADE_COUNT; i++) shadowPositions[i] = variables.shadowCascadeMatrix[i] * vec4(worldPosition, 1.0);

	//leafColor = vec3(0.0916, 0.1, 0.0125) * color * 0.75;
	//leafColor = vec3(0.0916, 0.1, 0.0125);
	leafColor.x = color;
	leafColor.y = 1.0 + Random11(vec4(localPosition, gl_VertexIndex)) * 0.5;

	gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);
}