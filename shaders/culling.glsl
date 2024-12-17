#ifndef CULLING_INCLUDED
#define CULLING_INCLUDED

layout(set = 0, binding = 4) uniform sampler2D cullSampler;

#include "transformation.glsl"
#include "functions.glsl"
#include "depth.glsl"

int InView(vec3 worldSpace, vec3 tolerance)
{
    vec3 clipSpacePosition = WorldToClip(worldSpace);

	bool xInView = clipSpacePosition.x >= 0.0 - tolerance.x && clipSpacePosition.x <= 1.0 + tolerance.x;
	bool yInView = clipSpacePosition.y >= 0.0 - tolerance.y && clipSpacePosition.y <= 1.0 + tolerance.y;
	bool zInView = clipSpacePosition.z >= 0.0 && clipSpacePosition.z <= 1.0;

	return ((xInView && yInView && zInView) ? 1 : 0);
}

int InView(vec3 worldSpace, float tolerance)
{
    return (InView(worldSpace, vec3(tolerance)));
}

int AreaInView(vec3 worldSpace, vec2 areaSize, float insideDis)
{
	vec3 rightOffset = variables.viewRight * areaSize.x;
	vec3 upOffset = variables.viewUp * areaSize.y;
	float inDistance = pow((areaSize.x * areaSize.y), insideDis);

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

int AreaInView(vec3 worldSpace, vec2 areaSize)
{
	return (AreaInView(worldSpace, areaSize, 1.5));
}

int Occluded(vec3 clipSpace, float tolerance)
{
	if (clipSpace.x > 1.0 || clipSpace.x < 0.0 || clipSpace.y > 1.0 || clipSpace.y < 0.0) return (0);

	float closestDepth = GetDepth(textureLod(cullSampler, clipSpace.xy, 0).r);

	float diff = clipSpace.z - closestDepth;

	if (diff > tolerance)
	{
		return (1);
	}
	else
	{
		return (0);
	}
}

int AreaOccluded(vec3 worldSpace, vec2 areaSize, float inDis, float tolerance)
{
	vec3 rightOffset = variables.viewRight * areaSize.x;
	vec3 upOffset = variables.viewUp * areaSize.y;
	//float inDistance = pow((areaSize.x * areaSize.y), inDis);

	//if (SquaredDistance(worldSpace, variables.viewPosition) <= inDistance) return (0);

	vec3 areaPosition = worldSpace - rightOffset - upOffset;
    if (Occluded(WorldToCull(areaPosition), tolerance) == 0) return (0);

	areaPosition = worldSpace + rightOffset + upOffset;
    if (Occluded(WorldToCull(areaPosition), tolerance) == 0) return (0);

	areaPosition = worldSpace + rightOffset - upOffset;
    if (Occluded(WorldToCull(areaPosition), tolerance) == 0) return (0);

	areaPosition = worldSpace - rightOffset + upOffset;
    if (Occluded(WorldToCull(areaPosition), tolerance) == 0) return (0);

	return (1);
}

int AreaOccluded(vec3 worldSpace, vec2 areaSize, float inDis)
{
	return (AreaOccluded(worldSpace, areaSize, inDis, 0));
}

int AreaOccluded(vec3 worldSpace, vec2 areaSize)
{
	return (AreaOccluded(worldSpace, areaSize, 1.5, 0));
}

#endif