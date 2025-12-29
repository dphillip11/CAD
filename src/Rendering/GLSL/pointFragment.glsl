#version 330 core

layout(std140) uniform GlobalUniforms
{
    vec4 pointColor;
    vec4 lineColor;
    vec4 faceColor;
    vec2 viewPortSize;
    float lineThickness;
    float axisLength;
};

in vec4 gColor;
out vec4 FragColor;

void main() {
    FragColor = gColor;
}
