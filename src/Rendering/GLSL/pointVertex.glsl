layout (location = 0) in vec3 aPos;

out vec3 worldPos;

void main() {
    worldPos = aPos;
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
}
