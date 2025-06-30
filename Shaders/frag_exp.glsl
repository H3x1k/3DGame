#version 330 core
out vec4 FragColor;

in vec3 FragPos;

void main() {
    FragColor = vec4(0.0, 0.3, 1.0, 1.0);
    
    /*vec3 ambient = vec3(0.1, 0.2, 0.3);

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 light = normalize(-lightDir); // direction TO the light
    float diff = pow(max(dot(norm, light), 0.0), 2);
    vec3 diffuse = diff * vec3(0.2, 0.4, 0.8);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-light, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = spec * vec3(1.0);

    vec3 waterColor = ambient + diffuse + specular;
    vec3 foamColor = vec3(0.95); // white foam

    // Blend based on foam amount
    //float foamFactor = clamp(FoamAmount / 1000000.0, 0.0, 1.0); // Adjust multiplier as needed
    //vec3 finalColor = mix(waterColor, foamColor, foamFactor);
    vec3 finalColor;
    //if (foamFactor > 0.5) {
    //    finalColor = vec3(1.0);
    //} else {
    //    finalColor = waterColor;
    //}
    finalColor = waterColor;

    FragColor = vec4(finalColor, 1.0);*/
}