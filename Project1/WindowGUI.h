#ifndef WINDOWGUI_H
#define WINDOWGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#include "Texture.h"
#include "Shape.h"

constexpr auto WINDOW_HEIGHT = 800;
constexpr auto WINDOW_WIDTH = 600;

struct Window {
    GLFWwindow* window = nullptr;
    uint32_t* shaderProgram = nullptr;
    uint32_t* texture = nullptr;
    uint32_t* VAO = nullptr;

    Window(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
    void run() const;

    ~Window() {}

};

namespace Transform_Methods {
    void initialize3DField(glm::mat4& viewMat, glm::mat4& projectionMat, glm::mat4& modelMat);
    void have_3D_object_rotate(const uint32_t& shaderProgram);
}

namespace WindowGUI_Methods {

    void framebuffer_size_callback(GLFWwindow* window, int height, int width);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    void insertShader(Window* window, uint32_t* shaderProgram);
    void insertVAO(Window* window, uint32_t* VAO);
    void insertTexture(Window* window, uint32_t* Texture);
}

#endif WINDOWGUI_H