#version 450

#extension GL_ARB_shading_language_include : require

layout(location = 0) in vec3 localCoordinates;

layout(location = 0) out vec4 outColor;

#include "functions.glsl"

const Triangle discardTriangle0 = Triangle(vec2(0.0, 0.25) * 1.5, vec2(0.25, -0.125) * 1.5, vec2(-0.25, -0.125) * 1.5);
const Triangle discardTriangle1 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(-0.125, 0.1875) * 1.425, vec2(-0.375, 0.0625) * 1.425);
const Triangle discardTriangle2 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(-0.125, -0.1875) * 1.425, vec2(-0.375, -0.0625) * 1.425);
const Triangle discardTriangle3 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(0.125, 0.1875) * 1.425, vec2(0.375, 0.0625) * 1.425);
const Triangle discardTriangle4 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(0.125, -0.1875) * 1.425, vec2(0.375, -0.0625) * 1.425);
const Triangle discardTriangle5 = Triangle(vec2(0.0, 0.0) * 1.425, vec2(0.25, 0.25) * 1.425, vec2(-0.25, 0.25) * 1.425);

bool ShouldDiscard(vec2 position)
{
	if (InsideTriangle(discardTriangle1, position)) return (true);
	else if (InsideTriangle(discardTriangle2, position)) return (true);
	else if (InsideTriangle(discardTriangle3, position)) return (true);
	else if (InsideTriangle(discardTriangle4, position)) return (true);
	else return (false);
}

void main()
{
    if (ShouldDiscard(localCoordinates.xz)) discard;

    outColor = vec4(1.0);
}