#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float uTime;

void main() {
    vec3 pos = aPos;
    pos.y = sin(10.0 * pos.x + uTime) * 0.1 + cos(10.0 * pos.z + uTime) * 0.1;
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
