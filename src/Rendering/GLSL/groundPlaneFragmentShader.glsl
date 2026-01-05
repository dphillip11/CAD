out vec4 FragColor;

in vec2 screenCoord;

void main() {
    // Unproject screen coordinates to ground plane (y=0)
    // screenCoord is in NDC space [-1, 1]
    
    // Get inverse view-projection matrix
    mat4 viewProj = projectionMatrix * viewMatrix;
    mat4 invViewProj = inverse(viewProj);
    
    // Create two points along the ray: near and far plane
    vec4 nearPoint = invViewProj * vec4(screenCoord, -1.0, 1.0);
    vec4 farPoint = invViewProj * vec4(screenCoord, 1.0, 1.0);
    
    // Perspective divide
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;
    
    // Ray direction
    vec3 rayDir = normalize(farPoint.xyz - nearPoint.xyz);
    vec3 rayOrigin = nearPoint.xyz;
    
    // Intersect with ground plane (y=0)
    // rayOrigin.y + t * rayDir.y = 0
    // t = -rayOrigin.y / rayDir.y
    
    if (rayDir.y >= 0.0) {
        // Ray pointing away from ground, discard
        discard;
    }
    
    float t = -rayOrigin.y / rayDir.y;
    
    if (t < 0.0) {
        // Intersection behind camera, discard
        discard;
    }
    
    vec3 groundPos = rayOrigin + t * rayDir;
    groundPos.y = 0.0; // Ensure exactly on ground plane
    
    // Set depth to far plane so geometry always wins depth test
    gl_FragDepth = 1.0;
    
    // Output raw world position (RGB32F texture)
    FragColor = vec4(groundPos, 1.0);
}
