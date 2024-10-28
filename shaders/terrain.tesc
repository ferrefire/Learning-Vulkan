#version 450 core

layout(set = 0, binding = 0) uniform Variables 
{
    vec3 viewPosition;
} variables;

//layout(vertices = 3) in;
layout(vertices = 3) out;

//uniform float tesselationFactor = 10;

float tesselationFactor = 10;

float TessellationFactor (vec3 p0, vec3 p1)
{
    float edgeLength = distance(p0, p1);
    vec3 edgeCenter = (p0 + p1) * 0.5;
    float viewDistance = distance(edgeCenter, variables.viewPosition);
    return (edgeLength * 540 * (1.0 / (tesselationFactor * viewDistance)));
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

		//float depth = GetWorldDepth(center);
		//float tolerance = pow(1.0 - depth, 3);

		bool cull = false;
		//bool cull = false;
		//if (depth < 0.25) cull = (
		//	AreaInView(center, vec2(20 * tolerance)) == 0 && 
        //    AreaInView(p0, vec2(20 * tolerance)) == 0 &&
        //    AreaInView(p1, vec2(20 * tolerance)) == 0 &&
        //    AreaInView(p2, vec2(20 * tolerance)) == 0);
		//else cull = (
		//	InView(center, 0) == 0 && 
        //    InView(p0, 0) == 0 &&
        //    InView(p1, 0) == 0 &&
        //    InView(p2, 0) == 0);

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