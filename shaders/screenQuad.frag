#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 1, binding = 0) uniform sampler2D textureSampler;

layout(location = 0) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"

void main()
{
    vec4 val = texture(textureSampler, inCoordinates);
    outColor = vec4(val.xyz, 1.0);

    //outColor = vec4(1);
}