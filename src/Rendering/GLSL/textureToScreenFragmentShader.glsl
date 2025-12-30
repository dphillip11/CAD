uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;

out vec4 FragColor;
in vec2 TexCoord;

void main() {
    vec4 color = vec4(0);

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
        vec2 position = TexCoord - vec2(1);
        vec4 t2 = texture(tex2, position);
        vec4 t1 = texture(tex1, position);
        vec4 t0 = texture(tex0, position);
        
        if (t0.a > 0.0) {
            color = t0;
        } else if (t1.a > 0.0) {
            color = t1;
        } else {
            color = t2;
        }
    }
    
    FragColor = color;
}