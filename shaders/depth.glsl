#ifndef DEPTH_INCLUDED
#define DEPTH_INCLUDED

//layout(set = 0, binding = 4) uniform sampler2D depthSampler;

#include "transformation.glsl"

float GetDepth(float z, float near, float far)
{
    float depth = z;

    depth = depth * 2.0 - 1.0;
    depth = (2.0 * near * far) / (far + near - depth * (far - near));
    depth = depth / far;
	//depth = clamp(depth, 0.0, 1.0);

    return (depth);
}

float GetDepth(float z)
{
    return (GetDepth(z, 0.1, 25000));
}

float GetWorldDepth(vec3 worldSpace)
{
	return (WorldToClip(worldSpace).z);
}

#endif