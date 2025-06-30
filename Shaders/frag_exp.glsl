#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform vec3 lightDir;

void main() {
    FragColor = vec4(0.0, 0.4, 1.0, 1.0);
    //FragColor = vec4(FragPos, 1.0);
}