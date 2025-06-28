#pragma once
#include <vector>
#include <glad/glad.h>

class PlaneMesh {
public:
    PlaneMesh(int resolution);
    ~PlaneMesh();
    void draw();

private:
    GLuint VAO, VBO, EBO;
    GLsizei indexCount;
};
