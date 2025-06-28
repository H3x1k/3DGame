#include "PlaneMesh.h"

PlaneMesh::PlaneMesh(int resolution) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Generate vertices
    for (int y = 0; y <= resolution; ++y) {
        for (int x = 0; x <= resolution; ++x) {
            float fx = (float)x / resolution;
            float fy = (float)y / resolution;
            vertices.push_back(fx * 2.0f - 1.0f); // X [-1, 1]
            vertices.push_back(0.0f);            // Y = 0 for flat plane
            vertices.push_back(fy * 2.0f - 1.0f); // Z [-1, 1]
        }
    }

    // Generate indices using triangle strips
    for (int y = 0; y < resolution; ++y) {
        for (int x = 0; x <= resolution; ++x) {
            indices.push_back(y * (resolution + 1) + x);
            indices.push_back((y + 1) * (resolution + 1) + x);
        }
        // Add degenerate triangles (skip if on last row)
        if (y < resolution - 1) {
            indices.push_back((y + 1) * (resolution + 1) + resolution);
            indices.push_back((y + 1) * (resolution + 1));
        }
    }

    indexCount = indices.size();

    // Create buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

PlaneMesh::~PlaneMesh() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void PlaneMesh::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
