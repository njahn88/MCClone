#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    // Get texture size
    ivec2 texSize = textureSize(ourTexture, 0);
    
    // Half-pixel offset in UV space (0.5 / 256 = 0.001953125)
    float offset = 0.5 / float(texSize.x); // Assuming square texture (texSize.x == texSize.y)
    
    // Adjust UV coordinates
    vec2 tc_final = TexCoord;
    // Adjust U-coordinate
    if (TexCoord.x == 0.0) {
        tc_final.x += offset;
    } else if (TexCoord.x == 1.0) {
        tc_final.x -= offset;
    }
    // Adjust V-coordinate
    if (TexCoord.y == 0.0) {
        tc_final.y += offset;
    } else if (TexCoord.y == 1.0) {
        tc_final.y -= offset;
    }
    
    // Sample texture with adjusted coordinates
    FragColor = texture(ourTexture, tc_final);
}