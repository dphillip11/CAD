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

void main() {  
    if (TexCoord.x < 0.5f && TexCoord.y < 0.5f)
    {
        FragColor = texture(tex0, 2.0f * TexCoord);
    }
    else if (TexCoord.x < 0.5f )
    {
        FragColor = texture(tex1, vec2(2.0f * TexCoord.x, TexCoord.y));
    }
    else if (TexCoord.y < 0.5f )
    {
        vec4 color = texture(tex2, vec2(TexCoord.x, 2.0f * TexCoord.y));
        uint faceId = decodeId(color.r, color.g);
        FragColor = faceId != 0u ? idToColor(faceId) : vec4(0);
    }
    else
    {
        FragColor = vec4(0);
    }
}