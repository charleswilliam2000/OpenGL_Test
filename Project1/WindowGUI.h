#ifndef WINDOWGUI_H
#define WINDOWGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Render.h"

namespace Callbacks {
    void framebuffer_size_callback(GLFWwindow* window, int height, int width);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}

class Window {
private:
    GLFWwindow* _window = nullptr;
    Camera _camera;
    Renderer* _renderer = nullptr;
public:
    Window(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
    void associateRenderer(Renderer& renderer);
    void insertCamera(Camera& camera);
    void run();

    ~Window() {}

};

#endif WINDOWGUI_H