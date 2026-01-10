out vec4 FragColor;
in vec2 TexCoord;

// Ray-plane intersection for ground plane at y=0
bool rayPlaneIntersect(vec3 rayOrigin, vec3 rayDir, out vec3 hitPoint) {
    float t = -rayOrigin.y / rayDir.y;
    if (t > 0.0) {
        hitPoint = rayOrigin + rayDir * t;
        return true;
    }
    return false;
}


// Get the fully composited pixel color at a given position
vec4 getCompositeColor(vec2 position) {
    // Reconstruct ray direction from NDC
    vec2 ndc = position * 2.0 - 1.0;
    
    // Inverse projection to get view space ray
    mat4 invProj = inverse(projectionMatrix);
    vec4 rayView = invProj * vec4(ndc, 1.0, 1.0);
    rayView /= rayView.w;
    
    // Inverse view to get world space ray
    mat4 invView = inverse(viewMatrix);
    vec3 rayOrigin = (invView * vec4(0, 0, 0, 1)).xyz;
    vec3 rayDir = normalize((invView * vec4(rayView.xyz, 0)).xyz);
    
    // Check for ground plane intersection
    vec3 hitPoint;
    float groundDepth = 1.0; // Far plane if no intersection
    
    if (rayPlaneIntersect(rayOrigin, rayDir, hitPoint)) {
        return vec4(hitPoint,1.0);
    } else {
        return vec4(0.0);
    }
}

void main() {    
    FragColor = getCompositeColor(TexCoord);
    gl_FragDepth = .99f;
}