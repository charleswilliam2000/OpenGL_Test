#include "WindowGUI.h"

void Window::associateRenderer(Renderer& renderer) {
    _renderer = &renderer;
}

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    : _window(glfwCreateWindow(width, height, title, monitor, share)) {
    //For future extensions, add here
    if (!_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Unable to initialize window!");
    }

    glfwMakeContextCurrent(_window);
    glfwSetKeyCallback(_window, Callbacks::key_callback);
    glfwSetFramebufferSizeCallback(_window, Callbacks::framebuffer_size_callback);
}

void Window::run() const {

    while (!glfwWindowShouldClose(_window)) {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _renderer->render();
            
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
    glfwDestroyWindow(_window);
}


void Callbacks::framebuffer_size_callback(GLFWwindow* window, int height, int width) {
    glViewport(0, 0, height, width);
}

void Callbacks::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    static bool wireframeMode = false;
    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        wireframeMode = !wireframeMode;

        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
            std::cout << "Wireframe mode ON\n";
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            std::cout << "Wireframe mode OFF\n";
        }
    }
}