layout (location = 0) in vec3 aPos;
layout (location = 1) in float aPrimitiveId;

out vec3 worldPos;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
    
    // Pass world position to fragment shader
    worldPos = aPos;
}
