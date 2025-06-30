#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
//out vec3 Normal;
//out float FoamAmount;

const int NUM_WAVES = 30;

uniform float Kx[NUM_WAVES];
uniform float Ky[NUM_WAVES];
uniform float W[NUM_WAVES];

vec3 Wave(vec2 position, vec2 K, float W, float t) {
    float Kmag = length(K);
    1.0 / Kmag / Kmag * exp(sin(K * position - W * t) - 1)
}

void main() {
    vec3 pos = aPos;
    vec3 totalOffset = vec3(0.0);
    //vec3 totalTangent = vec3(1.0, 0.0, 0.0);
    //vec3 totalBinormal = vec3(0.0, 0.0, 1.0);

    for (int i = 0; i < NUM_WAVES; ++i) {
        vec3 tangent, binormal;
        vec3 offset = gerstnerWave(
            amplitudes[i],
            wavelengths[i],
            speeds[i],
            directions[i],
            pos,
            uTime,
            tangent,
            binormal
        );
        totalOffset += offset;
        totalTangent += tangent;
        totalBinormal += binormal;
    }

    float foam = 0.0;

    for (int i = 0; i < NUM_WAVES; ++i) {
        float k = 6.28318 / wavelengths[i];
        vec2 d = normalize(directions[i]);
        float f = k * dot(d, pos.xz) - speeds[i] * k * uTime;
        float sinF = sin(f);

        float d2ydx2 = -amplitudes[i] * k * k * d.x * d.x * sinF;
        float d2ydz2 = -amplitudes[i] * k * k * d.y * d.y * sinF;

        float curvature = abs(d2ydx2 + d2ydz2); // Laplacian magnitude
        foam += curvature;
    }

    pos += totalOffset;
    vec3 normal = normalize(cross(totalBinormal, totalTangent));

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
    FoamAmount = foam;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}