#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 1, binding = 0) uniform sampler2D beamSamplers[1];

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "lighting.glsl"
#include "sampling.glsl"

void main()
{
    vec3 normal = normalize(inNormal);

    vec3 weights = GetWeights(normal, 1);

    vec3 color = SampleTriplanarColor(beamSamplers[0], worldPosition, weights);

    vec3 diffuse = DiffuseLighting(normal);

    vec3 finalLighting = color * diffuse;

    outColor = vec4(finalLighting, 1.0);
}