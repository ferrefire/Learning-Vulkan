#ifndef TRANSFORMATION_INCLUDED
#define TRANSFORMATION_INCLUDED

vec3 ObjectToWorld(vec3 objectSpace)
{
    vec3 worldSpace = (objectData.model * vec4(objectSpace, 1.0)).xyz;

    return (worldSpace);
}

#endif