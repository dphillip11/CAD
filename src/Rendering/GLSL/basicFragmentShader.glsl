out vec4 FragColor;

vec4 idToColor(uint id)
{
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

void main() {
#ifdef GL_ES
    // WebGL doesn't support gl_PrimitiveID, use solid color for now
    FragColor = faceColor;
#else
    // Desktop OpenGL supports gl_PrimitiveID
    // Sample the face ID from the 1D texture using gl_PrimitiveID
    uint faceId = texelFetch(faceIdTexture, ivec2(gl_PrimitiveID, 0), 0).r;
    
    // Convert face ID to a color
    FragColor = idToColor(faceId);
#endif
}