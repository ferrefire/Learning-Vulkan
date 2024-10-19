#version 450

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec2 outTexCoord;

void main() 
{
    outTexCoord = vec2(inPosition.xz + 0.5);
    gl_Position = ubo.projection * ubo.view * ubo.model * vec4(inPosition, 1.0);
}