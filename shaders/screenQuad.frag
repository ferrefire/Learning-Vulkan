#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "depth.glsl"

void main()
{
    float val = texture(textureSampler, inCoordinates).r;
    outColor = vec4(GetDepth(val));

    //outColor = vec4(1);
}