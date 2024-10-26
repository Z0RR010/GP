#version 330 core
out vec4 FragColor;
uniform vec3 color;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  
uniform sampler2D ourTexture;
uniform vec3 lightPos;
vec3 lightColor = vec3(1.0f);

void main()
{
    float ambientStrength = 0.5;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 result = (ambient + diffuse) * color;
    FragColor = texture(ourTexture, TexCoord) * vec4(result,1.0f);
} 