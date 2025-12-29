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
    vec4 color = vec4(0);

    // bottom left tex0
    if (TexCoord.x < 1.0f && TexCoord.y < 1.0f)
    {
        vec4 t0 = texture(tex0, TexCoord);
        // Debug: Add weak red tint to verify this is the bottom-left quadrant sampling tex0
        color = t0 + vec4(0.1, 0.0, 0.0, 0.0);
    }
    // bottom right tex1
    else if (TexCoord.y < 1.0f)
    {
        vec4 t1 = texture(tex1, vec2(TexCoord.x - 1.0f, TexCoord.y));
        // Debug: Add weak blue tint to verify this is the bottom-right quadrant sampling tex1
        // If t1 is empty/black, this will show as dark blue
        color = t1 + vec4(0.0, 0.0, 0.1, 0.0);
    }
    else 
    {
        // Top half - debug gray
        color = vec4(0.2, 0.2, 0.2, 1.0);
    }
    
    FragColor = color;
}