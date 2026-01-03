layout (location = 0) in vec3 aPos;
layout (location = 1) in float aPrimitiveId;

flat out uint vPrimitiveId;

void main() {
    gl_Position = projectionMatrix * viewMatrix * vec4(aPos, 1.0);
    
    // Pass through the primitive ID from vertex attribute
    vPrimitiveId = uint(aPrimitiveId);
}