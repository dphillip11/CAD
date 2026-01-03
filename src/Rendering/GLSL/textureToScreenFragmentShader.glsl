out vec4 FragColor;
in vec2 TexCoord;

// Decode two normalized floats back to a 16-bit ID (0-65535)
uint decodeId(float high, float low) {
    uint highBits = uint(high * 255.0 + 0.5);
    uint lowBits = uint(low * 255.0 + 0.5);
    return (highBits << 8u) | lowBits;
}

vec4 idToColor(uint id)
{
    if (int(id) == selectedFace) return vec4(1);

    const vec4 colors[10] = vec4[10](
        vec4(1.0, 0.3, 0.3, 1.0),  // Red
        vec4(0.3, 1.0, 0.3, 1.0),  // Green
        vec4(0.3, 0.3, 1.0, 1.0),  // Blue
        vec4(1.0, 1.0, 0.3, 1.0),  // Yellow
        vec4(1.0, 0.3, 1.0, 1.0),  // Magenta
        vec4(0.3, 1.0, 1.0, 1.0),  // Cyan
        vec4(1.0, 0.6, 0.3, 1.0),  // Orange
        vec4(0.6, 0.3, 1.0, 1.0),  // Purple
        vec4(0.3, 1.0, 0.6, 1.0),  // Mint
        vec4(1.0, 0.8, 0.5, 1.0)   // Peach
    );
    
    return colors[id % 10u];
}

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
// --- analytically box-filtered checkerboard (Inigo Quilez)
// filter kernel
float checkersTextureGradBox( in vec2 p, in vec2 ddx, in vec2 ddy )
{
    // filter kernel
    vec2 w = max(abs(ddx), abs(ddy)) + 0.01;
    // analytical integral (box filter)
    vec2 i = 2.0*(abs(fract((p-0.5*w)/2.0)-0.5)-abs(fract((p+0.5*w)/2.0)-0.5))/w;
    // xor pattern
    return 0.5 - 0.5*i.x*i.y;
}

// --- unfiltered checkerboard (fallback)
float checkersTexture( in vec2 p )
{
    vec2 q = floor(p);
    return mod( q.x+q.y, 2.0 );
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

// Get the fully composited pixel color at a given position
vec4 getCompositeColor(vec2 position) {
    vec4 t2 = texture(tex2, position);
    uint faceId = decodeId(t2.r, t2.g);
    t2 = idToColor(faceId);
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
        // Draw checkerboard on ground plane using analytic box-filtered sampling
        // We scale hitPoint.xz by 0.5 to match previous 2-unit squares
        vec2 p = hitPoint.xz * 0.5;
        // Compute texture-space derivatives for analytical filtering
        vec2 ddx = dFdx(p);
        vec2 ddy = dFdy(p);
        float checker = checkersTextureGradBox(p, ddx, ddy);
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
    
    // Sample depth and color from render passes
    float d0 = texture(depth0, position).r;
    float d1 = texture(depth1, position).r;
    float d2 = texture(depth2, position).r;
    
    // Start with background (ground/sky)
    vec4 color = backgroundColor;
    
    // Composite layers front-to-back with depth testing
    
    // Layer 2 (faces) - furthest back
    if (t2.a > 0.0 && d2 < groundDepth) {
        color = mix(color, t2, t2.a);
    }
    
    // Layer 1 (lines) - middle
    if (t1.a > 0.0 && d1 < groundDepth) {
        color = mix(color, t1, t1.a);
    }
    
    // Layer 0 (points) - front
    if (t0.a > 0.0 && d0 < groundDepth) {
        color = mix(color, t0, t0.a);
    }
    
    return color;
}

vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, p3.yzx+33.33);
    return fract((p3.xx+p3.yz)*p3.zy);
}

// Get color for a specific quadrant (single texture layer for debug views)
vec4 getQuadrantColor(int quadrant, vec2 position) {
    if (quadrant == 0) return texture(tex0, position);      // points
    if (quadrant == 1) return texture(tex1, position);      // lines
    if (quadrant == 2)
    {
        vec4 t2 = texture(tex2, position);
        uint faceId = decodeId(t2.r, t2.g);
        return idToColor(faceId);
    }
    return getCompositeColor(position);
}

void main() {
    // Determine which quadrant we're rendering and normalized position
    int quadrant;
    vec2 position;
    
    if (TexCoord.x < 1.0 && TexCoord.y < 1.0) {
        quadrant = 0;  // bottom left - points
        position = TexCoord;
    }
    else if (TexCoord.y < 1.0) {
        quadrant = 1;  // bottom right - lines
        position = vec2(TexCoord.x - 1.0, TexCoord.y);
    }
    else if (TexCoord.x < 1.0) {
        quadrant = 2;  // top left - faces
        position = vec2(TexCoord.x, TexCoord.y - 1.0);
    }
    else {
        quadrant = 3;  // top right - composite
        position = TexCoord - vec2(1.0);
    }
    
    FragColor = getQuadrantColor(quadrant, position);
}