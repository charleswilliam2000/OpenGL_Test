#include "WindowGUI.h"

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
    : window(glfwCreateWindow(width, height, title, monitor, share)) {
    //For future extensions, add here
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Unable to initialize window!");
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, WindowGUI_Methods::framebuffer_size_callback);
}

void Window::run() const {
    while (!glfwWindowShouldClose(window)) {
        WindowGUI_Methods::processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Shader_Methods::useShaderProgram(shaderProgram->_shaderProgram);
        glBindVertexArray(*VAO);
        glDrawElements(GL_TRIANGLES, Shape_Indices::Triangle, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
}

void WindowGUI_Methods::framebuffer_size_callback(GLFWwindow* window, int height, int width) {
    glViewport(0, 0, height, width);
}

void WindowGUI_Methods::processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void WindowGUI_Methods::insertShader(Window* window, ShaderProgram* shaderProgram) {
    window->shaderProgram = shaderProgram;
}

void WindowGUI_Methods::insertVAO(Window* window, uint32_t* VAO) {
    window->VAO = VAO;
}
