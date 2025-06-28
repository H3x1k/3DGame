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

glm::vec3 lightDir = glm::normalize(glm::vec3(-0.9f, -0.5f, -0.3f));

float yaw = 0.0f;
float pitch = 20.0f;
bool firstMouse = true;

float sensitivity = 0.1f;

const int NUM_WAVES = 30;

const float planeScale = 1.0f;

//const float R             = 0.60f; //  scale decay
//const float ALPHA         = 0.8f;  //  amplitude decay
//const float BETA          = 0.7f;  //  speed decay
//const float WAVELENGTH_0  = 60.0f; //  initial wavelength
//const float AMPLITUDE_0   = 1.5f;  //  initial amplitude
//const float SPEED_0       = 7.5f;  //  initial speed

//std::vector<float> amplitudes(NUM_WAVES);
//std::vector<float> wavelengths(NUM_WAVES);
//std::vector<float> speeds(NUM_WAVES);
//std::vector<glm::vec2> directions(NUM_WAVES);
std::vector<float> A(NUM_WAVES);
std::vector<float> K(NUM_WAVES);
std::vector<float> W(NUM_WAVES);

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

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    //GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Black Hole Simulation", monitor, nullptr);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Water Simulation", nullptr, nullptr);
    glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_FALSE);
    glfwSetWindowPos(window, 0, 0);
    //GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint shaderProgram = LoadShaderProgram("../Shaders/vert.glsl", "../Shaders/frag.glsl");

    PlaneMesh plane(256);


    auto hash = [](float n) -> float {
        return glm::fract(sin(n) * 43758.5453123f);
        };

    /*for (int i = 0; i < NUM_WAVES; ++i) {
        float wavelength = WAVELENGTH_0 * pow(R, i);
        float amplitude = AMPLITUDE_0 * pow(R, ALPHA * i);
        float speed = SPEED_0 * pow(R, BETA * i);

        float k = 6.28318f / wavelength;
        if (k * amplitude >= 1.0f)
            amplitude = 0.9f / k;

        float angle = hash(float(i)) * 6.28318f;
        glm::vec2 direction = glm::vec2(cos(angle), sin(angle));

        amplitudes[i] = amplitude;
        wavelengths[i] = wavelength;
        speeds[i] = speed;
        directions[i] = direction;
    }

    glUseProgram(shaderProgram);

    glUniform1fv(glGetUniformLocation(shaderProgram, "amplitudes"), NUM_WAVES, amplitudes.data());
    glUniform1fv(glGetUniformLocation(shaderProgram, "wavelengths"), NUM_WAVES, wavelengths.data());
    glUniform1fv(glGetUniformLocation(shaderProgram, "speeds"), NUM_WAVES, speeds.data());
    glUniform2fv(glGetUniformLocation(shaderProgram, "directions"), NUM_WAVES, &directions[0].x);*/


    float lastFrame = 0.0f;
    float startTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout << 1 / deltaTime << std::endl;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (height == 0) height = 1;
        float aspect = static_cast<float>(width) / height;

        processInput(window, deltaTime);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        
        // Model Matrix
        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(planeScale, 1.0f, planeScale));

        // View Matrix
        glm::mat4 yawMat = glm::rotate(glm::mat4(1.0), glm::radians(yaw), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 pitchMat = glm::rotate(glm::mat4(1.0), glm::radians(pitch), cameraRight);
        glm::mat4 rotation = yawMat * pitchMat;
        glm::mat4 view = glm::inverse(rotation * glm::translate(glm::mat4(1.0f), -cameraPos));

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Projection Matrix
        glm::mat4 projection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 100.0f);

        // Sending Uniforms to GPU
        // Vertex Shader Uniforms
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1f(glGetUniformLocation(shaderProgram, "uTime"), glfwGetTime());
        // Fragment Shader Uniforms
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));
        
        // Draw
        plane.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
