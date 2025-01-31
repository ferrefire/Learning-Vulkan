#ifndef TRANSFORMATION_INCLUDED
#define TRANSFORMATION_INCLUDED

//const float far = 25000;
//const float farMult = 0.00004;

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
    vec4 viewSpace = variables.viewMatrix * vec4(worldSpace, 1.0);

    vec3 clipSpace = viewSpace.xyz;
    clipSpace /= viewSpace.w;

    clipSpace.xy = clipSpace.xy * 0.5 + 0.5;
    clipSpace.z = viewSpace.w * variables.ranges.w;

    return (clipSpace);
}

vec3 WorldToCull(vec3 worldSpace)
{
    vec4 viewSpace = variables.cullMatrix * vec4(worldSpace, 1.0);

    vec3 clipSpace = viewSpace.xyz;
    clipSpace /= viewSpace.w;

    clipSpace.xy = clipSpace.xy * 0.5 + 0.5;
    clipSpace.z = viewSpace.w * variables.ranges.w;

    return (clipSpace);
}

mat4 GetRotationMatrix(float angle, vec3 axis)
{
    //axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 Rotate(vec3 vec, float angle, vec3 axis)
{
	return (GetRotationMatrix(angle, axis) * vec4(vec, 0.0)).xyz;
}

#endif