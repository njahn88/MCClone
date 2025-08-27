#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 lightPos;


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

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * lightColor;

    // Darken texture
    FragColor = texture(ourTexture, tc_final) * vec4(result, 1.0);
    //FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0);
}