#version 330 core
layout (location = 0) in vec3 aPos;

layout(std140) uniform GlobalUniforms
{
    vec4 lineColor;
    vec4 faceColor;
};

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    gl_Position = vec4(aPos, 1.0);
}