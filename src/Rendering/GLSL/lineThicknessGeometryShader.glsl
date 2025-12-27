#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float uThickness = 0.25f;     // line thickness in pixels
uniform vec2 uViewportSize;   // viewport size in pixels (width, height)

void main()
{
    // Clip-space positions
    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;

    // Convert to NDC
    vec2 ndc0 = p0.xy / p0.w;
    vec2 ndc1 = p1.xy / p1.w;

    // Line direction in screen space
    vec2 dir = normalize(ndc1 - ndc0);

    // Perpendicular direction
    vec2 normal = vec2(-dir.y, dir.x);

    // Convert thickness from pixels to NDC
    vec2 pixelToNDC = vec2(2.0) / uViewportSize;
    vec2 offset = normal * uThickness * 0.5 * pixelToNDC;

    // Emit quad (triangle strip)
    gl_Position = vec4((ndc0 + offset) * p0.w, p0.z, p0.w);
    EmitVertex();

    gl_Position = vec4((ndc0 - offset) * p0.w, p0.z, p0.w);
    EmitVertex();

    gl_Position = vec4((ndc1 + offset) * p1.w, p1.z, p1.w);
    EmitVertex();

    gl_Position = vec4((ndc1 - offset) * p1.w, p1.z, p1.w);
    EmitVertex();

    EndPrimitive();
}
