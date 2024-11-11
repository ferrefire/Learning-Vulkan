#version 450 core

#extension GL_ARB_shading_language_include : require

//#define OBJECT_DATA_COUNT 25
//layout(set = 1, binding = 0) uniform ObjectData
//{
//    mat4 model;
//} objectDatas[OBJECT_DATA_COUNT];
//
//layout(push_constant, std430) uniform PushConstants
//{
//    uint chunkIndex;
//} pc;

//layout(vertices = 3) in;
layout(vertices = 3) out;

#define SAMPLE_COUNT 3

#include "variables.glsl"
#include "culling.glsl"
#include "depth.glsl"
#include "functions.glsl"

const float tesselationFactor = 10;
const float toleranceFactor = 0.025;
const float cullFactor = 0.0025;

int PatchOccluded(vec3 p0, vec3 p1, vec3 p2, vec3 center)
{
	if (Occluded(WorldToCull(center), 0.01) == 0) return (0);
	else if (Occluded(WorldToCull(p0), 0.01) == 0) return (0);
	else if (Occluded(WorldToCull(p1), 0.01) == 0) return (0);
	else if (Occluded(WorldToCull(p2), 0.01) == 0) return (0);

	return (1);
}

float TessellationFactor (vec3 p0, vec3 p1)
{
    float edgeLength = distance(p0, p1);
    vec3 edgeCenter = (p0 + p1) * 0.5;
    float viewDistance = distance(edgeCenter, variables.viewPosition);
    return (edgeLength * variables.resolution.y * (1.0 / (tesselationFactor * viewDistance)));
}

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if (gl_InvocationID == 0)
    {
        vec3 p0 = (gl_in[0].gl_Position).xyz;
        vec3 p1 = (gl_in[1].gl_Position).xyz;
        vec3 p2 = (gl_in[2].gl_Position).xyz;

        vec3 center = (p0 + p1 + p2) * (1.0 / 3.0);

		float depth = GetWorldDepth(center);
		float tolerance = pow(1.0 - depth, 3);
		bool cull = false;
		if (depth < 0.25) cull = (AreaInView(center, vec2(20 * tolerance)) == 0 && AreaInView(p0, vec2(20 * tolerance)) == 0 && AreaInView(p1, vec2(20 * tolerance)) == 0 && AreaInView(p2, vec2(20 * tolerance)) == 0);
        else cull = (InView(center, 0) == 0 && InView(p0, 0) == 0 && InView(p1, 0) == 0 && InView(p2, 0) == 0);

		if (variables.occlusionCulling == 1 && !cull) cull = PatchOccluded(p0, p1, p2, center) == 1;

        //if (!cull)
        //{
        //    vec3 offset = vec3(0, 100 * depth, 0);
        //    vec3 centerClip = WorldToCull(center + offset);
        //    vec3 p0Clip = WorldToCull(p0 + offset);
        //    vec3 p1Clip = WorldToCull(p1 + offset);
        //    vec3 p2Clip = WorldToCull(p2 + offset);
        //    cull = Occluded(centerClip, 0.01) == 1;
        //    if (cull) cull = Occluded(p0Clip, 0.01) == 1;
        //    if (cull) cull = Occluded(p1Clip, 0.01) == 1;
        //    if (cull) cull = Occluded(p2Clip, 0.01) == 1;
        //}

        /*bool cull = true;
        //float sqrDis = SquaredDistance(variables.viewPosition, center);
        //if (sqrDis > 2500) sqrDis = SquaredDistance(variables.viewPosition, p0);
        //if (sqrDis > 2500) sqrDis = SquaredDistance(variables.viewPosition, p1);
        //if (sqrDis > 2500) sqrDis = SquaredDistance(variables.viewPosition, p2);
        //if (sqrDis <= 2500) cull = false;

        vec3 centerClip = WorldToClip(center);
        vec3 p0Clip = WorldToClip(p0);
        vec3 p1Clip = WorldToClip(p1);
        vec3 p2Clip = WorldToClip(p2);

        float closestZ = 10;
        if (centerClip.z > 0.0 && centerClip.z < closestZ) closestZ = centerClip.z;
        if (p0Clip.z > 0.0 && p0Clip.z < closestZ) closestZ = p0Clip.z;
        if (p1Clip.z > 0.0 && p1Clip.z < closestZ) closestZ = p1Clip.z;
        if (p2Clip.z > 0.0 && p2Clip.z < closestZ) closestZ = p2Clip.z;

        if (cull && closestZ >= 0.0 && closestZ <= 1.0)
        {
            vec4 bounds = vec4(0.0, 1.0, 0.0, 1.0);
            //if (cull) cull = PointInSquare(bounds, WorldToClip(center).xy) == 0;
            if (cull) cull = PointInSquare(bounds, center.xy) == 0;
            if (cull) cull = PointInSquare(bounds, p0Clip.xy) == 0;
            if (cull) cull = PointInSquare(bounds, p1Clip.xy) == 0;
            if (cull) cull = PointInSquare(bounds, p2Clip.xy) == 0;

            if (cull)
            {
                float tf = mix(toleranceFactor, 0.0001, closestZ);
                //float tf = 0.001;

                if (cull) cull = PointInTriangle(p0Clip.xy, p1Clip.xy, p2Clip.xy, vec2(0.0, 0.0), tf) == 0;
                if (cull) cull = PointInTriangle(p0Clip.xy, p1Clip.xy, p2Clip.xy, vec2(1.0, 1.0), tf) == 0;
                if (cull) cull = PointInTriangle(p0Clip.xy, p1Clip.xy, p2Clip.xy, vec2(0.0, 1.0), tf) == 0;
                if (cull) cull = PointInTriangle(p0Clip.xy, p1Clip.xy, p2Clip.xy, vec2(1.0, 0.0), tf) == 0;
            }

            if (cull)
            {
                Triangle triangle;
                triangle.p0 = p0Clip.xy;
                triangle.p1 = p1Clip.xy;
                triangle.p2 = p2Clip.xy;

                Square square;
                square.p0 = vec2(0.0);
                square.p1 = vec2(0.0, 1.0);
                square.p2 = vec2(1.0);
                square.p3 = vec2(1.0, 0.0);

                if (cull) cull = LineIntersectsTriangle(triangle, GetLine(square, 0)) == 0;
                if (cull) cull = LineIntersectsTriangle(triangle, GetLine(square, 1)) == 0;
                if (cull) cull = LineIntersectsTriangle(triangle, GetLine(square, 2)) == 0;
                if (cull) cull = LineIntersectsTriangle(triangle, GetLine(square, 3)) == 0;
            }

            //if (!cull)
            //{
            //    float cf = mix(cullFactor, 0.025, closestZ);
            //    cull = Occluded(WorldToCull(center), cf) == 1;
            //    if (cull) cull = Occluded(WorldToCull(p0), cf) == 1;
            //    if (cull) cull = Occluded(WorldToCull(p1), cf) == 1;
            //    if (cull) cull = Occluded(WorldToCull(p2), cf) == 1;
            //}

            //if (cull) cull = CullPatch(p0, p1, p2, center) == 1;
        }*/

        if (cull)
        {
            gl_TessLevelOuter[0] = 0;
            gl_TessLevelOuter[1] = 0;
            gl_TessLevelOuter[2] = 0;
            gl_TessLevelInner[0] = 0;
        }
		else
		{
			float tessLevel1 = TessellationFactor(p1, p2);
        	float tessLevel2 = TessellationFactor(p2, p0);
        	float tessLevel3 = TessellationFactor(p0, p1);

        	gl_TessLevelOuter[0] = tessLevel1;
        	gl_TessLevelOuter[1] = tessLevel2;
        	gl_TessLevelOuter[2] = tessLevel3;

        	gl_TessLevelInner[0] = (tessLevel1 + tessLevel2 + tessLevel3) * (1.0 / 3.0);
		}
    }
}