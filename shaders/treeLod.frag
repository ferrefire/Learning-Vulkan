#version 450

#extension GL_ARB_shading_language_include : require

#ifndef CASCADE_COUNT
#define CASCADE_COUNT 3
#endif

#ifndef SIDE_COUNT
#define SIDE_COUNT 8
#endif

layout(set = 1, binding = 2) uniform sampler2D treeDiffuseSampler;
layout(set = 1, binding = 3) uniform sampler2D treeLodSamplers[SIDE_COUNT];

layout(location = 0) in vec2 inCoord;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 worldPosition;
//layout(location = 3) in vec4 shadowPositions[CASCADE_COUNT];

layout(location = 0) out vec4 outColor;

const vec3 trunkColor = (vec3(156, 121, 70) / 255.0);
const vec3 trunkLodColor = (vec3(48, 32, 16) / 255.0);
const float sideBlendStrength = 4.0;

#include "variables.glsl"
#include "lighting.glsl"
#include "depth.glsl"
#include "functions.glsl"
#include "transformation.glsl"

void main()
{
    vec3 normal = inNormal;
    normal = normalize(normal);
    vec2 uv = inCoord;
    
    if (!gl_FrontFacing)
    {
        uv.x = 1.0 - uv.x;
        normal *= -1;
    }
    //if (!gl_FrontFacing) discard;

    //outColor = vec4(normal * 0.5 + 0.5, 1.0);
    ////outColor = vec4(vec3(1.0, 0.0, 0.0) * 0.5 + 0.5, 1.0);
    //return;
    
    //float signX = sign(normal.x);
    //float signZ = sign(normal.z);
    //vec3 weights = abs(normal);
    //weights = NormalizeSum(normal);
    //weights = pow(weights, vec3(sideBlendStrength));
	//weights = NormalizeSum(weights);
    //vec4 diffuseZ = texture(treeLodSamplers[(signZ < 0.0 ? 0 : 2)], inCoord).rgba;
    //vec4 diffuseX = texture(treeLodSamplers[(signX < 0.0 ? 1 : 3)], inCoord).rgba;
    //vec4 diffuse = diffuseZ * weights.z + diffuseX * weights.x;

    float angleStep = 360.0 / float(SIDE_COUNT);
    int closestIndex = 0;
    float closestDot = 0.0;
    for (int i = 0; i < SIDE_COUNT; i++)
    {
        vec3 sampleDirection = normalize(Rotate(vec3(0.0, 0.0, -1.0), radians(angleStep * float(i)), vec3(0.0, 1.0, 0.0)));
        float directionDot = (dot(normal, sampleDirection));

        if (directionDot > closestDot)
        {
            closestDot = directionDot;
            closestIndex = i;
        }
    }

    //vec4 diffuse;
    //if (diffuseZ.a < 1.0) diffuse = diffuseX;
    //else if (diffuseX.a < 1.0) diffuse = diffuseZ;
    //else diffuse = diffuseZ * abs(weights.z) + diffuseX * abs(weights.x);

    //if (diffuse.r > 0.1 && diffuse.g > 0.1 && diffuse.b > 0.1) discard;

    vec4 diffuse = texture(treeLodSamplers[closestIndex], uv).rgba;
    if (diffuse.a < 1.0) discard;

	//float depth = GetDepth(gl_FragCoord.z);

	float shadow = GetTerrainShadow(worldPosition.xz);
	//if (shadow < 1.0)
	//	shadow = clamp(shadow + GetCascadedShadow(shadowPositions, depth), 0.0, 1.0);

	vec3 combinedColor = diffuse.rgb * clamp(1.0 - shadow, 0.025, 1.0);
	//vec3 combinedColor = pow(diffuse.rgb, vec3(1.0 / 1.25));
    //if (closestIndex != 3) combinedColor = vec3(0.0);

	outColor = vec4(combinedColor, 1.0);
}