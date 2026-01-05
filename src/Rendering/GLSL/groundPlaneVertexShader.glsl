layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 screenCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    // Pass through NDC coordinates for fragment shader
    screenCoord = aPos.xy;
}
