#version 450

layout(location = 0) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

void main() 
{
    //vec4 color = texture(texSampler, fragTexCoord);
    //float brightness = (color.r + color.g + color.b) / 3.0;
    //outColor = vec4(vec3(brightness), 1);
    outColor = texture(texSampler, fragTexCoord);
}