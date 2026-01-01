// Simple point fragment shader for WebGL
out vec4 FragColor;

void main() {
    // Draw a circular point by discarding fragments outside the circle
    vec2 coord = gl_PointCoord - vec2(0.5);
    if (length(coord) > 0.5) {
        discard;
    }
    FragColor = pointColor;
}
