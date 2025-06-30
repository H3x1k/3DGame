#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec3 Normal;
out float secDeriv;

const int NUM_WAVES = 50;

uniform float scale;
uniform float hscale;
uniform float Kx[NUM_WAVES];
uniform float Ky[NUM_WAVES];
uniform float W[NUM_WAVES];
uniform float P[NUM_WAVES];

float Wave(vec2 position, vec2 K, float W, float P, float t) {
    float Kmag = length(K);
    return exp(sin(dot(K, position) - W * t - P) - 1.0) / Kmag / Kmag;
}

void main() {
    vec3 pos = aPos;
    float heightOffset = 0.0;

    float dx = 0.0;
    float dz = 0.0;
    float dxx = 0.0;
    float dyy = 0.0;

    for (int i = 0; i < NUM_WAVES; i++) {
        vec2 Ki = vec2(Kx[i], Ky[i]);
        vec2 position = vec2(pos.x, pos.z) * scale;

        float a = Wave(position, Ki, W[i], P[i], uTime);
        float cos = cos(dot(Ki, position) - W[i] * uTime - P[i]);
        float sin = sin(dot(Ki, position) - W[i] * uTime - P[i]);

        heightOffset += a;
        dx += a * cos * Kx[i];
        dz += a * cos * Ky[i];
        dxx += a * cos * cos * Kx[i] * Kx[i] - a * sin * Kx[i] * Kx[i];
        dyy += a * cos * cos * Ky[i] * Ky[i] - a * sin * Ky[i] * Ky[i];
    }

    Normal = normalize(vec3(-dx, 1/hscale, -dz));
    secDeriv = (dxx + dyy) * heightOffset * heightOffset * sign(heightOffset) * hscale;

    pos += vec3(0.0, heightOffset, 0.0);

    FragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}