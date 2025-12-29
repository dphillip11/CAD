#version 330 core
out vec4 FragColor;

layout(std140) uniform GlobalUniforms
{
    vec4 pointColor;
    vec4 lineColor;
    vec4 faceColor;
    vec2 viewPortSize;
    float lineThickness;
    float axisLength;
};

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    FragColor = lineColor;
}
