layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

in vec3 worldPos[];
out vec4 gColor;

// Helper function to emit a thick line as a triangle strip
void emitThickLine(vec3 center, vec3 axis, vec4 color) {
    // Calculate the line direction
    vec3 lineDir = normalize(axis);
    vec3 start = center - axis;
    vec3 end = center + axis;
    
    // Get camera forward direction in world space
    vec3 viewDir = normalize((inverse(viewMatrix) * vec4(0.0, 0.0, -1.0, 0.0)).xyz);
    
    // Calculate perpendicular direction for thickness (billboard towards camera)
    vec3 right = normalize(cross(lineDir, viewDir));
    vec3 up = normalize(cross(lineDir, right));
    
    // Create a quad with thickness in world space
    float halfThickness = axisLength * 0.05; // Proportional to axis length
    
    gColor = color;
    
    // Triangle strip for the quad (4 vertices)
    // Bottom-left
    gl_Position = projectionMatrix * viewMatrix * vec4(start - right * halfThickness, 1.0);
    EmitVertex();
    
    // Bottom-right
    gl_Position = projectionMatrix * viewMatrix * vec4(start + right * halfThickness, 1.0);
    EmitVertex();
    
    // Top-left
    gl_Position = projectionMatrix * viewMatrix * vec4(end - right * halfThickness, 1.0);
    EmitVertex();
    
    // Top-right
    gl_Position = projectionMatrix * viewMatrix * vec4(end + right * halfThickness, 1.0);
    EmitVertex();
    
    EndPrimitive();
    
    // Add another quad perpendicular for better visibility from all angles
    // Using 'up' direction instead of 'right'
    
    // Bottom-left
    gl_Position = projectionMatrix * viewMatrix * vec4(start - up * halfThickness, 1.0);
    EmitVertex();
    
    // Bottom-right
    gl_Position = projectionMatrix * viewMatrix * vec4(start + up * halfThickness, 1.0);
    EmitVertex();
    
    // Top-left
    gl_Position = projectionMatrix * viewMatrix * vec4(end - up * halfThickness, 1.0);
    EmitVertex();
    
    // Top-right
    gl_Position = projectionMatrix * viewMatrix * vec4(end + up * halfThickness, 1.0);
    EmitVertex();
    
    EndPrimitive();
}

void main()
{
    vec3 center = worldPos[0];

    // Emit red line along x axis (world space)
    emitThickLine(center, vec3(axisLength, 0.0, 0.0), vec4(1.0, 0.0, 0.0, 1.0));

    // Emit green line along y axis (world space)
    emitThickLine(center, vec3(0.0, axisLength, 0.0), vec4(0.0, 1.0, 0.0, 1.0));

    // Emit blue line along z axis (world space)
    emitThickLine(center, vec3(0.0, 0.0, axisLength), vec4(0.0, 0.0, 1.0, 1.0));
}
