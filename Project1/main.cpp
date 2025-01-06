#define STB_IMAGE_IMPLEMENTATION
#include "Buffers.h"
#include "WindowGUI.h"

int main() {
    try {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            throw std::runtime_error("Unable to initialize GLFW");
        }

        Window window(Constants::WINDOW_HEIGHT, Constants::WINDOW_WIDTH, "OpenGL_window");

        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD";
            throw std::runtime_error("Unable to initialize GLAD");
        }

        glEnable(GL_DEPTH_TEST);

        BufferObjects bufferObjects(Shapes::cube_vertices, Shapes::cube_indices);
        Texture texture("wall.jpg");

        Renderer renderer = buildRenderer(bufferObjects._VAO, texture._textureID);
        window.associateRenderer(renderer);
        window.run();

        Buffers_Methods::terminateBufferObjects(bufferObjects);
        renderer.terminateShaderProgram();
    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    glfwTerminate();
    return 0;
}
