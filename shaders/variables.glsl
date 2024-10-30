#ifndef VARIABLES_INCLUDED
#define VARIABLES_INCLUDED

layout(set = 0, binding = 0) uniform Variables 
{
	mat4 view;
	mat4 projection;
    vec3 viewPosition;
    vec3 viewDirection;
    vec3 viewRight;
    vec3 viewUp;
	vec4 resolution;
} variables;

#endif