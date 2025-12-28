#version 330 core

layout(std140) uniform GlobalUniforms
{
    vec4 lineColor;
    vec4 faceColor;
};

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 FragColor;
in vec2 TexCoord;

void main() {
    FragColor = texture(tex0, TexCoord);
}