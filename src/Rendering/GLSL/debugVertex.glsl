layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main() {
    vec4 position = vec4(aPos, 1.0);
    position.z -= 0.1f;
    gl_Position = position;
    TexCoord = aTexCoord;
}
