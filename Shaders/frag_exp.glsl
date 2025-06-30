#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in float secDeriv;

uniform vec3 viewPos;
uniform vec3 lightDir;

void main() {
    vec3 ambient = vec3(0.1, 0.2, 0.3);
    //vec3 waterColor = vec3(0.0, 0.4, 1.0);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir); // direction TO the light
    float diff = pow(max(dot(norm, light), 0.0), 2);
    vec3 diffuse = diff * vec3(0.2, 0.4, 0.8);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, Normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(0.9);

    // Foam
    float foam = max(-secDeriv / 100.0, 0.0);
    vec3 foamColor = vec3(1.0);

    vec3 waterColor = ambient + diffuse + specular;
    vec3 finalColor = mix(waterColor, foamColor, foam);

    FragColor = vec4(finalColor, 1.0);
}