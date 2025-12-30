layout(points) in;
layout(line_strip, max_vertices = 6) out;

out vec4 gColor;

void main()
{
    vec4 center = gl_in[0].gl_Position;

    // Emit red line along x axis
    gColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
    gl_Position = center - vec4(axisLength, 0.0, 0.0, 0.0);
    EmitVertex();
    gl_Position = center + vec4(axisLength, 0.0, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();

    // Emit green line along y axis
    gColor = vec4(0.0, 1.0, 0.0, 1.0); // Green
    gl_Position = center - vec4(0.0, axisLength, 0.0, 0.0);
    EmitVertex();
    gl_Position = center + vec4(0.0, axisLength, 0.0, 0.0);
    EmitVertex();
    EndPrimitive();

    // Emit blue line along z axis
    gColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue
    gl_Position = center - vec4(0.0, 0.0, axisLength, 0.0);
    EmitVertex();
    gl_Position = center + vec4(0.0, 0.0, axisLength, 0.0);
    EmitVertex();
    EndPrimitive();
}
