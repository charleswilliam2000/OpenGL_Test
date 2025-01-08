#ifndef WINDOWGUI_H
#define WINDOWGUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Render.h"

namespace Callbacks {
    void framebuffer_size_callback(GLFWwindow* window, int height, int width);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
}

class Screen {
private:
    GLFWwindow* _window = nullptr;
    Camera* camera = nullptr;
    Renderer* renderer = nullptr;

public:
    bool wireframeMode = false;

    Screen(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
    void run() const;

    void associateRenderer(Renderer& in_renderer);
    void insertCamera(Camera& in_camera);

    void processMouseMovement(double xOffset, double yOffset);

    ~Screen() {}

};

#endif WINDOWGUI_H