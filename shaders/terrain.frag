#version 450

layout(binding = 1) uniform sampler2D heightMapSampler;
layout(binding = 2) uniform sampler2D grassSampler;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    //outColor = vec4(1);

	//float height = texture(heightMapSampler, inTexCoord).r * 2;
	float height = 1;
    outColor = texture(grassSampler, inPosition.xz * 0.2) * vec4(vec3(height), 1);

    //outColor = texture(heightMapSampler, inTexCoord);
}