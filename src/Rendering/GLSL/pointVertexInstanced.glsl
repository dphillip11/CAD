// Simple point rendering for WebGL (no geometry shader support)
layout (location = 0) in vec3 aPos;

out vec3 worldPos;

void main() {
    worldPos = aPos;
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
    // Set point size based on axisLength
    // Note: WebGL may clamp this to a maximum value
    gl_PointSize = max(5.0, axisLength * 100.0);
}
