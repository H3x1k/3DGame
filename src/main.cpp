#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "PlaneMesh.h";
#include "Shader.h";

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = 0.0f;
float pitch = 20.0f;
bool firstMouse = true;

float sensitivity = 0.1f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 800.0f / 2.0;
    static float lastY = 600.0f / 2.0;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    float sinyaw = sin(glm::radians(yaw));
    float cosyaw = cos(glm::radians(yaw));
    float sinpit = sin(glm::radians(pitch));
    float cospit = cos(glm::radians(pitch));

    /*glm::vec3 frontdir;
    frontdir.x = sinyaw;
    frontdir.y = 0.0f;
    frontdir.z = -cosyaw;
    cameraFront = frontdir;*/

    glm::vec3 frontdir;
    frontdir.x = sinyaw * cospit;
    frontdir.y = sinpit;
    frontdir.z = -cosyaw * cospit;
    cameraFront = frontdir;

    glm::vec3 rightdir;
    rightdir.x = cosyaw;
    rightdir.y = 0.0f;
    rightdir.z = sinyaw;
    cameraRight = rightdir;
}
void processInput(GLFWwindow* window, float deltaTime) {
    const float cameraSpeed = 4.0f * deltaTime; // time-adjusted movement speed

    glm::vec3 movementVector = glm::vec3(0.0f, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movementVector += cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movementVector -= cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        movementVector -= cameraRight;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        movementVector += cameraRight;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        movementVector += cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        movementVector -= cameraUp;

    float mag = glm::length(movementVector);
    if (mag > 0)
        cameraPos += movementVector / mag * cameraSpeed * (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ? 2.0f : 1.0f);
}

std::string loadShaderSource(const char* path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << '\n';
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compilation failed ("
            << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment")
            << "):\n" << infoLog << '\n';
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vCode = loadShaderSource(vertexPath);
    std::string fCode = loadShaderSource(fragmentPath);

    if (vCode.empty() || fCode.empty()) {
        std::cerr << "Shader source code is empty.\n";
        return 0;
    }

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vCode.c_str());
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fCode.c_str());

    if (vs == 0 || fs == 0) {
        std::cerr << "Shader compilation failed, program not created.\n";
        return 0;
    }

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Shader program linking failed:\n" << infoLog << '\n';
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Black Hole Simulation", monitor, nullptr);
    //GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLint shaderProgram = LoadShaderProgram("../Shaders/vert.glsl", "../Shaders/frag.glsl");

    PlaneMesh plane(256);

    float lastFrame = 0.0f;
    float startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout << 1 / deltaTime << std::endl;

        processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        float timeValue = glfwGetTime();
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), timeValue);
        
        // Model Matrix
        glm::mat4 model = glm::mat4(1.0f);

        // View Matrix
        glm::mat4 yawMat = glm::rotate(glm::mat4(1.0), glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0), glm::radians(pitch), cameraRight);
        glm::mat4 rotation = yawMat * pitchMat;
        glm::mat4 view = glm::inverse(rotation * glm::translate(glm::mat4(1.0f), -cameraPos));

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Projection Matrix
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);

        // Sending Uniforms to GPU
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), glfwGetTime());
        
        // Draw
        plane.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
