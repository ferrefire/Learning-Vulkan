#version 450

#extension GL_ARB_shading_language_include : require

struct GrassData
{
    vec3 position;
    vec3 rotation;
    vec3 normal;
};

layout(std430, set = 1, binding = 0) buffer DataBuffer
{
	GrassData data[];
};

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 worldPosition;
layout(location = 1) out vec3 objectNormal;
layout(location = 2) out vec3 terrainNormal;
layout(location = 3) out vec3 grassColor;
layout(location = 4) out vec2 uv;

const uint grassCount = 128;
const float grassCountMult = 1.0 / 128.0;
const uint grassTotalCount = 128 * 128;

const float spacing = 0.25;
const float spacingMult = 4;

#include "variables.glsl"
#include "functions.glsl"
#include "transformation.glsl"

void main()
{
	vec3 position = data[gl_InstanceIndex].position + variables.viewPosition;
	float ran = data[gl_InstanceIndex].rotation.x;

	float squaredDistance = SquaredDistance(position, variables.viewPosition);
	float maxDistance = pow(grassCount * spacing, 2);
	float maxDistanceMult = pow(grassCountMult * spacingMult, 2);

	float scale = clamp(squaredDistance, 0.0, maxDistance) * maxDistanceMult;
	scale = 1.0 - pow(1.0 - scale, 4);
	scale = 1.0 + scale * 2;
	vec3 scaledPosition = inPosition * scale;

	vec3 normal = vec3(0, 0, 1);

	float angle = radians(ran * (inPosition.y + 0.25));
	scaledPosition = Rotate(scaledPosition, angle, vec3(1, 0, 0));
	normal = Rotate(normal, angle, vec3(1, 0, 0));

	ran = data[gl_InstanceIndex].rotation.y;

	angle = radians(ran);
	scaledPosition = Rotate(scaledPosition, angle, vec3(0, 1, 0));
	normal = Rotate(normal, angle, vec3(0, 1, 0));

	objectNormal = normal;

	worldPosition = ObjectToWorld(scaledPosition, mat4(1)) + position;

	terrainNormal = data[gl_InstanceIndex].normal;
    gl_Position = variables.projection * variables.view * vec4(worldPosition, 1.0);

	grassColor = vec3(0.25, 0.6, 0.1);
	uv = vec2(inPosition.x * 10 + 0.5, inPosition.y);
}