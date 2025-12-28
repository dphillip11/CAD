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
    vec4 color = texture(tex0, TexCoord);
    if (color.x < 0.01 && color.y < 0.01 && color.z < 0.01)
    {
        color.w = 0.0;
    }
    FragColor = color;
}