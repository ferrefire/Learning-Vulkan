#ifndef CULLING_INCLUDED
#define CULLING_INCLUDED

#include "transformation.glsl"
#include "functions.glsl"

const float far = 25000;

int InView(vec3 worldSpace, vec3 tolerance)
{
    vec3 clipSpacePosition = WorldToClip(worldSpace);

	bool xInView = clipSpacePosition.x > 0.0 - tolerance.x && clipSpacePosition.x < 1.0 + tolerance.x;
	bool yInView = clipSpacePosition.y > 0.0 - tolerance.y && clipSpacePosition.y < 1.0 + tolerance.y;
	bool zInView = clipSpacePosition.z > 0.0 && clipSpacePosition.z < far;

	return ((xInView && yInView && zInView) ? 1 : 0);
}

int InView(vec3 worldSpace, float tolerance)
{
    return (InView(worldSpace, vec3(tolerance)));
}

int AreaInView(vec3 worldSpace, vec2 areaSize)
{
	vec3 rightOffset = variables.viewRight * areaSize.x;
	vec3 upOffset = variables.viewUp * areaSize.y;
	float inDistance = pow((areaSize.x * areaSize.y) * 2, 2);

	if (SquaredDistance(worldSpace, variables.viewPosition) <= inDistance) return (1);

	vec3 areaPosition = worldSpace - rightOffset - upOffset;
    if (InView(areaPosition, 0) == 1) return (1);

	areaPosition = worldSpace + rightOffset + upOffset;
    if (InView(areaPosition, 0) == 1) return (1);

	areaPosition = worldSpace + rightOffset - upOffset;
    if (InView(areaPosition, 0) == 1) return (1);

	areaPosition = worldSpace - rightOffset + upOffset;
    if (InView(areaPosition, 0) == 1) return (1);

	return (0);
}

#endif