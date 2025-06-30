#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;

const int NUM_WAVES = 10;

uniform float Kx[NUM_WAVES];
uniform float Ky[NUM_WAVES];
uniform float W[NUM_WAVES];

vec3 Wave(vec2 position, vec2 K, float W, float t) {
    float Kmag = length(K);
    return 1.0 / Kmag / Kmag * exp(sin(K * position - W * t) - 1);
}

void main() {
    vec3 pos = aPos;
    vec3 totalOffset = vec3(0.0);

    for (int i = 0; i < NUM_WAVES; ++i) {
        vec3 offset = Wave(aPos.xy, vec2(Kx[i], Ky[i]), W[i], uTime);
        totalOffset += offset;
    }

    pos += totalOffset;

    FragPos = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}