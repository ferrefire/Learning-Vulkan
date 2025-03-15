#version 450

#extension GL_ARB_shading_language_include : require

struct LeafData
{
	uint posxz;
	uint posyroty;
	uint scalxrotx;
	uint colxnormx;
	uint normyz;
	uint rotz;
};

layout(std430, set = 1, binding = 0) readonly buffer DataBuffer
{
	LeafData data[];
};

layout(push_constant, std430) uniform PushConstants
{
	uint shadowCascade;
} pc;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 localCoordinates;

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

struct RotateResults
{
	vec3 position;
	vec3 normal;
};

RotateResults RotateLeaf(vec3 objectPosition, vec3 rotations, float scale)
{
	vec3 scaledPosition = inPosition * scale;
	mat4 rotationMatrix = GetRotationMatrix(radians(rotations.y), vec3(0.0, 1.0, 0.0));
	rotationMatrix = rotationMatrix * GetRotationMatrix(radians(rotations.x * 0.5), vec3(1.0, 0.0, 0.0));
	//rotationMatrix = rotationMatrix * GetRotationMatrix(radians(rotations.z * 0.25), vec3(0.0, 0.0, 1.0));
    vec3 rotatedPosition = (rotationMatrix * vec4(scaledPosition, 1.0)).xyz;
	//vec3 rotatedNormal = normalize((rotationMatrix * vec4(vec3(0, 1, 0), 1.0)).xyz);

	RotateResults results;
	results.position = rotatedPosition;
	//results.normal = rotatedNormal;
	return (results);
}

void main()
{
	vec3 position = vec3(0);
	vec3 rotation = vec3(0);
	float scale = 1.0;

	uint dataIndex = gl_InstanceIndex;

	//position = data[dataIndex].position;

	position.xz = unpackHalf2x16(data[dataIndex].posxz);
	vec2 posyroty = unpackHalf2x16(data[dataIndex].posyroty);
	position.y = posyroty.x;
	rotation.y = posyroty.y * 360.0 - 180.0;
	vec2 scalxrotx = unpackHalf2x16(data[dataIndex].scalxrotx);
	scale = scalxrotx.x;
	rotation.x = scalxrotx.y * 360.0 - 180.0;
	vec2 rotz = unpackHalf2x16(data[dataIndex].rotz);
	rotation.z = rotz.x * 360.0 - 180.0;

	position += variables.viewPosition;

	//vec3 objectPosition = inPosition * scale;
	//mat4 rotationMatrix = GetRotationMatrix(radians(rotation.y), vec3(0.0, 1.0, 0.0));
	//rotationMatrix = rotationMatrix * GetRotationMatrix(radians(rotation.x * 0.5), vec3(1.0, 0.0, 0.0));
	////rotationMatrix = rotationMatrix * GetRotationMatrix(radians(rotation.z * 0.25), vec3(0.0, 0.0, 1.0));
    //objectPosition = (rotationMatrix * vec4(objectPosition, 1.0)).xyz;
	////vec3 localNormal = normalize((rotationMatrix * vec4(vec3(0, 1, 0), 1.0)).xyz);
	////normal = (rotationMatrix * vec4(vec3(0, 1, 0), 1.0)).xyz;

	RotateResults rotateResults = RotateLeaf(inPosition, rotation, scale);

	//vec3 localNormal = rotateResults.normal;
	vec3 worldPosition = ObjectToWorld(rotateResults.position, mat4(1)) + position;

	/*vec3 viewDirection = normalize(worldPosition - variables.viewPosition);
	float viewDotNormal = dot(localNormal, viewDirection);
	//float rotateFactor = sign(viewDotNormal) - clamp(viewDotNormal * 4.0, -1.0, 1.0);
	float rotateFactor = (1.0 - abs(clamp(viewDotNormal * 1.0, -1.0, 1.0)));

	if (abs(rotateFactor) > 0.01)
	{
		vec3 weights = abs(localNormal);
		weights = NormalizeSum(weights);

		rotation.y += rotateFactor * 90.0 * (1.0 - weights.y);
		rotation.x += rotateFactor * 90.0 * weights.y;

		rotateResults = RotateLeaf(inPosition, rotation, scale);

		localNormal = rotateResults.normal;
		worldPosition = ObjectToWorld(rotateResults.position, mat4(1)) + position;
	}*/

	//vec3 worldPosition = ObjectToWorld(objectPosition, mat4(1)) + position;

	//leafColor = vec3(0.0916, 0.1, 0.0125) * color;

	//gl_Position = variables.viewMatrix * vec4(worldPosition, 1.0);

	localCoordinates = inPosition;

	gl_Position = variables.shadowCascadeMatrix[pc.shadowCascade] * vec4(worldPosition, 1.0);
}