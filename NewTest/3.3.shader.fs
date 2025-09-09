#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    // Sample texture with adjusted coordinates
    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}