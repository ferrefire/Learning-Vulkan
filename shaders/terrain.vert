#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(binding = 1) uniform sampler2D heightMapSampler;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 outPosition;
layout(location = 1) out vec2 outTexCoord;

void main() 
{
    outTexCoord = vec2(inPosition.xz + 0.5);
    vec3 position = inPosition;
    position.y += texture(heightMapSampler, outTexCoord).r * 0.25;
	outPosition = (ubo.model * vec4(position, 1.0)).xyz;
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(position, 1.0);
}