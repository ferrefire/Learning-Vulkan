#ifndef CULLING_INCLUDED
#define CULLING_INCLUDED

#include "transformation.glsl"

const float far = 25000;

int InView(vec3 position, vec3 tolerance)
{
    vec3 clipSpacePosition = WorldToClip(position);

	bool xInView = clipSpacePosition.x > 0.0 - tolerance.x && clipSpacePosition.x < 1.0 + tolerance.x;
	bool yInView = clipSpacePosition.y > 0.0 - tolerance.y && clipSpacePosition.y < 1.0 + tolerance.y;
	bool zInView = clipSpacePosition.z > 0.0 && clipSpacePosition.z < far;

	return ((xInView && yInView && zInView) ? 1 : 0);
}

int InView(vec3 position, float tolerance)
{
    return (InView(position, vec3(tolerance)));
}

#endif