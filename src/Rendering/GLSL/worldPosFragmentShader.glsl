out vec4 FragColor;

in vec3 worldPos;

void main() {
    // Output raw world position as floats (RGB32F texture)
    // RGB = world position, A = 1.0 for valid geometry
    FragColor = vec4(worldPos, 1.0);
}
