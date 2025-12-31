layout (location = 0) in vec3 aPos;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
}