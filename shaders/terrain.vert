#version 450

layout(set = 0, binding = 0) uniform Variables 
{
    vec3 viewPosition;
} variables;

layout(set = 1, binding = 0) uniform ObjectData 
{
    mat4 model;
    mat4 view;
    mat4 projection;
} objectData;

layout(set = 1, binding = 1) uniform sampler2D heightMapSampler;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexCoord;

void main()
{
    outTexCoord = vec2(inPosition.xz + 0.5);
    vec3 position = inPosition;
    position.y += texture(heightMapSampler, outTexCoord).r * 0.25;
    //position.y += texture(heightMapSampler, vec2(inPosition.xz + 0.5)).r * 0.25;
	outPosition = (objectData.model * vec4(position, 1.0)).xyz;
	//if (distance(outPosition.xz, variables.viewPosition.xz + vec2(0, 1000)) < 100) position.y += 1;
    gl_Position = objectData.projection * objectData.view * objectData.model * vec4(position, 1.0);
}