uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D depth0;
uniform sampler2D depth1;
uniform sampler2D depth2;

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

// Checkerboard pattern
float checkerboard(vec2 pos, float scale) {
    vec2 p = floor(pos * scale);
    return mod(p.x + p.y, 2.0);
}

// Sky gradient with horizon
vec3 skyColor(float rayY) {
    // Horizon at rayY = 0, sky above (positive), ground below (negative)
    float t = smoothstep(-0.1, 0.3, rayY);
    vec3 horizonColor = vec3(0.7, 0.75, 0.8);  // Light blue-grey
    vec3 skyColor = vec3(0.4, 0.5, 0.7);       // Darker blue above
    vec3 groundFog = vec3(0.6, 0.65, 0.7);     // Ground fog color
    
    if (rayY > 0.0) {
        return mix(horizonColor, skyColor, t);
    } else {
        return mix(groundFog, horizonColor, smoothstep(-0.3, 0.0, rayY));
    }
}

void main() {
    vec4 color = vec4(0);

    // Sample the three render textures and composite
    // bottom left tex0
    if (TexCoord.x < 1.0f && TexCoord.y < 1.0f)
    {
        color = texture(tex0, TexCoord);
    }
    // bottom right tex1
    else if (TexCoord.y < 1.0f)
    {
        color = texture(tex1, vec2(TexCoord.x - 1.0f, TexCoord.y));
    }
    else if (TexCoord.x < 1.0f)
    {
        color = texture(tex2, vec2(TexCoord.x, TexCoord.y - 1.0f));
    }
    else
    {
        // Main viewport - composite all layers with ground plane
        vec2 position = TexCoord - vec2(1);
        vec4 t2 = texture(tex2, position);
        vec4 t1 = texture(tex1, position);
        vec4 t0 = texture(tex0, position);
        
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
        vec4 backgroundColor;
        float groundDepth = 1.0; // Far plane if no intersection
        
        if (rayPlaneIntersect(rayOrigin, rayDir, hitPoint)) {
            // Draw checkerboard on ground plane
            float checker = checkerboard(hitPoint.xz, 0.5);  // 2 unit squares
            vec3 color1 = vec3(0.4, 0.4, 0.4);  // Dark grey
            vec3 color2 = vec3(0.7, 0.7, 0.7);  // Light grey
            vec3 groundColor = mix(color1, color2, checker);
            
            // Fade to horizon with distance
            float dist = length(hitPoint.xz);
            float fade = exp(-dist * 0.05);
            groundColor = mix(skyColor(0.0), groundColor, fade);
            
            backgroundColor = vec4(groundColor, 1.0);
            
            // Calculate ground plane depth in clip space
            vec4 groundClip = projectionMatrix * viewMatrix * vec4(hitPoint, 1.0);
            groundDepth = (groundClip.z / groundClip.w) * 0.5 + 0.5;
        } else {
            // Sky gradient
            backgroundColor = vec4(skyColor(rayDir.y), 1.0);
        }
        
        // Sample depth values from render passes
        float d0 = texture(depth0, position).r;
        float d1 = texture(depth1, position).r;
        float d2 = texture(depth2, position).r;
        
        // Composite render layers with depth testing against ground plane
        // Only show ground where depth indicates no geometry in front
        if (t0.a > 0.0 && d0 < groundDepth) {
            color = t0;
        } else if (t1.a > 0.0 && d1 < groundDepth) {
            color = t1;
        } else if (t2.a > 0.0 && d2 < groundDepth) {
            color = t2;
        } else {
            color = backgroundColor;
        }
    }
    
    FragColor = color;
}