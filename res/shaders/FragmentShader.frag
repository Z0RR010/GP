#version 460 core
struct Light {
    int type;           // 0: Directional, 1: Point, 2: Spotlight
    int on;            // If false, the light is ignored
    vec3 position;      // Used for point and spotlights
    vec3 direction;     // Used for directional and spotlights
    vec3 color;         // Light color
    float intensity;    // Intensity for all light types
    float cutoff;       // Spotlight cutoff (cosine of the inner cone angle)
    float outerCutoff;  // Spotlight outer cutoff (cosine of the outer cone angle)
};
out vec4 FragColor;


in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform bool generated;
uniform float constantAttenuation;
uniform float linearAttenuation;
uniform float quadraticAttenuation;
uniform vec3 viewPos;
uniform Light lights[4];
uniform float ambientStrength;
uniform vec3 ambientColor;
uniform samplerCube skybox;
uniform bool reflection;
uniform float refractRatio;
uniform vec3 color;


void main()
{
    vec3 ambient = ambientStrength * ambientColor;

    //vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(lightPos - FragPos);
    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = diff * lightColor;
    vec3 result = vec3(0.0f);// = (ambient + diffuse) * color;
    result += ambient;
    vec4 base;
    base = texture(texture_diffuse1, TexCoord);
    if (generated)
    {
        base = vec4(color,1.0f);
    }
    //else
    //{
        // * texture(texture_specular1, TexCoord);// * vec4(result,1.0f);
    //}
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    for (int i = 0; i < 4; ++i) {
        if (lights[i].on == 0) continue; // Skip disabled lights

        vec3 lightColor = lights[i].color * lights[i].intensity;
        
        
        if (lights[i].type == 0) { // Directional light
            vec3 lightDir = normalize(-lights[i].direction);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = lightColor * diff;
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Shininess
            vec3 specular = lightColor * spec;

            result += diffuse + specular;
            

        } else { // Point light or Spotlight
            vec3 lightDir = normalize(lights[i].position - FragPos);
            float distance = length(lights[i].position - FragPos);

            // Attenuation factor
            float attenuation = 1.0 / (constantAttenuation + 
                                       linearAttenuation * distance + 
                                       quadraticAttenuation * distance * distance);

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = lightColor * diff;

            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
            vec3 specular = lightColor * spec;

            if (lights[i].type == 2) { // Spotlight-specific behavior
                float theta = dot(lightDir, normalize(-lights[i].direction));
                float epsilon = lights[i].cutoff - lights[i].outerCutoff;
                float spotFactor = clamp((theta - lights[i].outerCutoff) / epsilon, 0.0, 1.0);
                attenuation *= spotFactor; // Apply spotlight factor to attenuation
            }

            result += attenuation * (diffuse + specular);
        }
    }

    vec4 col = vec4(result,1.0f) * base;
    float gamma = 2.2;
    
    if (reflection)
    {
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = reflect(I, normalize(Normal));
        col.rgb = (texture(skybox, R).rgb * result);
    }
    if (refractRatio != 1.0)
    {
        float ratio = 1.00 / refractRatio;
        vec3 I = normalize(FragPos - viewPos);
        vec3 R = refract(I, normalize(Normal), ratio);
        col.rgb = (texture(skybox, R).rgb) * (result);
    }
    col.rgb = pow(col.rgb, vec3(1.0/gamma));
    //FragColor = vec4(texture(skybox, R).rgb, 1.0);
    FragColor = col;
    //FragColor = vec4(1.0f);
} 