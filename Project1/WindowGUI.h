#ifndef WINDOWGUI_H
#define WINDOWGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"
#include "Shape.h"

struct Window {
    GLFWwindow* window = nullptr;
    ShaderProgram* shaderProgram = nullptr;
    uint32_t* texture = nullptr;
    uint32_t* VAO = nullptr;

    Window(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
    void run() const;

    ~Window() {}

};

namespace WindowGUI_Methods {

    void framebuffer_size_callback(GLFWwindow* window, int height, int width);
    void processInput(GLFWwindow* window);

    void insertShader(Window* window, ShaderProgram* shaderProgram);
    void insertVAO(Window* window, uint32_t* VAO);
    void insertTexture(Window* window, uint32_t* Texture);
}

#endif WINDOWGUI_H