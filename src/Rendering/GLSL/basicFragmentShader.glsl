out vec4 FragColor;

flat in uint vPrimitiveId;

// Split 32-bit ID into two 8-bit components and encode as normalized floats
void encodeId(uint id, out float high, out float low) {
    uint highBits = (id >> 8u) & 0xFFu;  // Upper 8 bits
    uint lowBits = id & 0xFFu;            // Lower 8 bits
    high = float(highBits) / 255.0;
    low = float(lowBits) / 255.0;
}

void main() {
    // 16-bit version (for IDs up to 65535):
    float high, low;
    encodeId(vPrimitiveId, high, low);
    FragColor = vec4(high, low, 0, 1.0);
}