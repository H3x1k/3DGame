#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 lightDir;

void main() {
    vec3 ambient = vec3(0.1, 0.2, 0.3);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir); // direction TO the light
    float diff = max(dot(norm, light), 0.0);
    vec3 diffuse = diff * vec3(0.2, 0.4, 0.8);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(1.0);

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, 1.0);
    //FragColor = vec4(FragPos.y, FragPos.y, 1.0, clamp(1 - FragPos.y, 0.2, 1.0));
}