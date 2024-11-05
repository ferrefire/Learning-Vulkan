#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

#include "variables.glsl"
#include "heightmap.glsl"
#include "depth.glsl"

void main()
{
    //vec4 val = texture(depthSampler, inCoordinates);
    //outColor = val;

    outColor = vec4(1);
}