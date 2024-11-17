#version 460 core
out vec4 FragColor;
uniform vec3 color;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 lightPos;
uniform bool generated;
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
    if (generated)
    {
        FragColor = vec4(color,1.0f);
    }
    else
    {
        FragColor = texture(texture_diffuse1, TexCoord);// * texture(texture_specular1, TexCoord);// * vec4(result,1.0f);
    }
    
    //FragColor = vec4(1.0f);
} 