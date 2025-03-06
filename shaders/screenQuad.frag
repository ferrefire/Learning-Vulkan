#version 450

#extension GL_ARB_shading_language_include : require

layout(set = 1, binding = 0) uniform sampler2D textureSampler;
//layout(set = 1, binding = 0) uniform sampler3D textureSampler;

layout(location = 0) in vec2 inCoordinates;

layout(location = 0) out vec4 outColor;

void main()
{
	discard;
    //vec3 val = vec3(texture(textureSampler, inCoordinates).r);
    vec3 val = texture(textureSampler, inCoordinates).rgb;

	//int xi = int(floor(inCoordinates.x / 0.125));
	//float x = (inCoordinates.x - (xi * 0.125)) * 8.0;
	//int yi = int(floor(inCoordinates.y / 0.25));
	//float y = (inCoordinates.y - (yi * 0.25)) * 4.0;
	//float z = (yi * 8.0 + xi) / 32.0;
	//vec3 uv = vec3(x, y, z);
	////uv = vec3(inCoordinates, 0.0);
    //vec3 val = texture(textureSampler, uv).rgb;
    //vec3 val = vec3(texture(textureSampler, uv).a);

    //outColor = vec4(GetDepth(val));

	//{-1, -1, -1, 1}, { 1, -1, -1, 1}, { 1,  1, -1, 1}, {-1,  1, -1, 1},
    //    {-1, -1,  1, 1}, { 1, -1,  1, 1}, { 1,  1,  1, 1}, {-1,  1,  1, 1}

	//float closest = 10;
	//vec4 camFrustum = vec4(-1, -1, -1, 1);
	//camFrustum = variables.frustumMatrix * camFrustum;
	//camFrustum /= camFrustum.w;
	//camFrustum *= 0.5 + 0.5;
	//camFrustum = variables.shadowLod1Projection * variables.shadowLod1View * vec4(camFrustum.xyz, 1.0);
	//camFrustum /= camFrustum.w;
	//camFrustum *= 0.5 + 0.5;
	//float tempDis = distance(camFrustum.xy, inCoordinates.xy);
	//vec3 depthVal = vec3(GetDepth(val, 50.0, 250.0));
	//vec3 endVal = mix(vec3(0), depthVal, );

	//vec3 finalColor = vec3(GetDepth(val, 1.0, 50.0));

	//for (int i = 0; i < 4; i++)
	//{
	//	if (distance(inCoordinates, variables.frustumCorners[i] * 0.5 + 0.5) < 0.01)
	//	{
	//		finalColor = vec3(0);
	//	}
	//}

	//if (distance(inCoordinates, (variables.shadowCascadeMatrix[1] * vec4(variables.viewPosition, 1.0)).xy * 0.5 + 0.5) < 0.01) finalColor = vec3(0);
	//if (distance(inCoordinates, variables.frustumCorner1.xy * 0.5 + 0.5) < 0.01) finalColor = vec3(0);
	//else if (distance(inCoordinates, variables.frustumCorner2.xy * 0.5 + 0.5) < 0.01) finalColor = vec3(0);
	//else if (distance(inCoordinates, variables.frustumCorner3.xy * 0.5 + 0.5) < 0.01) finalColor = vec3(0);
	//else if (distance(inCoordinates, variables.frustumCorner4.xy * 0.5 + 0.5) < 0.01) finalColor = vec3(0);

    outColor = vec4(val, 1.0);

    //outColor = vec4(1);
}