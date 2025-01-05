#define STB_IMAGE_IMPLEMENTATION
#include "Buffers.h"
#include "WindowGUI.h"

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

        glEnable(GL_DEPTH_TEST);

        ShaderProgram shaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
        BufferObjects bufferObjects(Shapes::cube_vertices, Shapes::cube_indices);
        Texture texture("wall.jpg");

        WindowGUI_Methods::insertShader(&window, &shaderProgram._shaderProgram);
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
