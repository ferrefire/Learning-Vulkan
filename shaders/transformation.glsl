#ifndef TRANSFORMATION_INCLUDED
#define TRANSFORMATION_INCLUDED

vec3 ObjectToWorld(vec3 objectSpace, mat4 model)
{
    vec3 worldSpace = (model * vec4(objectSpace, 1.0)).xyz;

    return (worldSpace);
}

vec4 WorldToView(vec3 worldSpace)
{
    vec4 viewSpace = variables.view * vec4(worldSpace, 1.0);

    return (viewSpace);
}

vec3 WorldToClip(vec3 worldSpace)
{
    vec4 viewSpace = variables.projection * WorldToView(worldSpace);

    vec3 clipSpace = viewSpace.xyz;
    clipSpace /= viewSpace.w;

    clipSpace.x = clipSpace.x * 0.5 + 0.5;
    clipSpace.y = clipSpace.y * 0.5 + 0.5;
    clipSpace.z = viewSpace.w;

    return (clipSpace);
}

#endif