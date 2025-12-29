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

layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = vec4(aPos, 1.0);
}
