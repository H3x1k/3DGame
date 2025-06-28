#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec3 Normal;

void main() {
    vec3 pos = aPos;
    pos.y += sin(10.0 * pos.x + uTime) * 0.1 + cos(10.0 * pos.z + uTime) * 0.1;

    // Approximate normal by calculating partial derivatives
    float delta = 0.01;
    float heightL = sin(10.0 * (aPos.x - delta) + uTime) * 0.1 + cos(10.0 * aPos.z + uTime) * 0.1;
    float heightR = sin(10.0 * (aPos.x + delta) + uTime) * 0.1 + cos(10.0 * aPos.z + uTime) * 0.1;
    float heightD = sin(10.0 * aPos.x + uTime) * 0.1 + cos(10.0 * (aPos.z - delta) + uTime) * 0.1;
    float heightU = sin(10.0 * aPos.x + uTime) * 0.1 + cos(10.0 * (aPos.z + delta) + uTime) * 0.1;

    vec3 dx = vec3(2.0 * delta, heightR - heightL, 0.0);
    vec3 dz = vec3(0.0, heightU - heightD, 2.0 * delta);
    vec3 normal = normalize(cross(dz, dx));

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}