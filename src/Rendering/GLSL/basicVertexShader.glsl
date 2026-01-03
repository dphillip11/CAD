layout (location = 0) in vec3 aPos;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
}