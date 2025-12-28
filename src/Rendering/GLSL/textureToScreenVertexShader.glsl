#version 330 core

layout(std140) uniform GlobalUniforms
{
    vec4 lineColor;
    vec4 faceColor;
};

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
