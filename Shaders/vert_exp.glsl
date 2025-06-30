#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec3 Normal;

const int NUM_WAVES = 10;

uniform float Kx[NUM_WAVES];
uniform float Ky[NUM_WAVES];
uniform float W[NUM_WAVES];

float Wave(vec2 position, vec2 K, float W, float t) {
    float Kmag = length(K);
    return exp(sin(dot(K, position) - W * t) - 1.0) / Kmag / Kmag;
}

void main() {
    vec3 pos = aPos;
    float heightOffset = 0.0;

    for (int i = 0; i < NUM_WAVES; i++) {
        if (Kx[i] != 0.0 || Ky[i] != 0.0)
            heightOffset += Wave(vec2(pos.x, pos.z), vec2(Kx[i], Ky[i]), W[i], uTime);
    }

    float dx = 0.0;
    float dz = 0.0;

    for (int i = 0; i < NUM_WAVES; i++) {
        vec2 K = vec2(Kx[i], Ky[i]);
        vec2 position = vec2(pos.x, pos.z);
        float w = Wave(position, K, W[i], uTime) * cos(dot(K, position) - W * t)
        dx += w * Kx[i];
        dz += w * Ky[i];
    }

    Normal = normalize(vec3(dx, 1, dz));

    pos += vec3(0.0, heightOffset, 0.0);

    FragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}