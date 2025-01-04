#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "Buffers.h"
#include "Texture.h"
#include "WindowGUI.h"

constexpr auto WINDOW_HEIGHT = 800;
constexpr auto WINDOW_WIDTH = 600;

int main() {
    try {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            throw std::runtime_error("Unable to initialize GLFW");
        }

        Window window(WINDOW_HEIGHT, WINDOW_WIDTH, "OpenGL_window");

        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD";
            throw std::runtime_error("Unable to initialize GLAD");
        }

        ShaderProgram shaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
        BufferObjects bufferObjects(Shapes::rectangle_vertices, Shapes::rectangle_indices);
        Texture texture("wall.jpg");

        WindowGUI_Methods::insertShader(&window, &shaderProgram);
        WindowGUI_Methods::insertVAO(&window, &bufferObjects._VAO);
        WindowGUI_Methods::insertTexture(&window, &texture._textureID);

        window.run();

        Buffers_Methods::terminateBufferObjects(bufferObjects);
        glDeleteProgram(shaderProgram._shaderProgram);
    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    glfwTerminate();
    return 0;
}
