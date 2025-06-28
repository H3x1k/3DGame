#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

out vec3 FragPos;
out vec3 Normal;

const int NUM_WAVES = 30;

uniform float amplitudes[NUM_WAVES];
uniform float wavelengths[NUM_WAVES];
uniform float speeds[NUM_WAVES];
uniform vec2 directions[NUM_WAVES];

struct Wave {
    float amplitude;
    float wavelength;
    float speed;
    vec2 direction;
};

vec3 gerstnerWave(float amplitude, float wavelength, float speed, vec2 direction, vec3 position, float time, out vec3 tangent, out vec3 binormal) {
    float k = 6.28318 / wavelength;
    vec2 d = normalize(direction);
    float f = k * dot(d, position.xz) - speed * k * time;

    float cosF = cos(f);
    float sinF = sin(f);

    tangent = vec3(1.0 - d.x * d.x * amplitude * k * cosF,
                   d.x * amplitude * k * sinF,
                   -d.x * d.y * amplitude * k * cosF);
    binormal = vec3(-d.x * d.y * amplitude * k * cosF,
                    d.y * amplitude * k * sinF,
                    1.0 - d.y * d.y * amplitude * k * cosF);

    return vec3(d.x * amplitude * cosF, amplitude * sinF, d.y * amplitude * cosF);
}

void main() {
    vec3 pos = aPos;
    vec3 totalOffset = vec3(0.0);
    vec3 totalTangent = vec3(1.0, 0.0, 0.0);
    vec3 totalBinormal = vec3(0.0, 0.0, 1.0);

   

    // this could be wrong
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

    pos += totalOffset;
    vec3 normal = normalize(cross(totalBinormal, totalTangent)); // this too

    FragPos = vec3(model * vec4(pos, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}