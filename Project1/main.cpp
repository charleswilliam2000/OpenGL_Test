#define STB_IMAGE_IMPLEMENTATION
#include "Buffers.h"
#include "WindowGUI.h"

int main() {
    try {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            throw std::runtime_error("Unable to initialize GLFW");
        }

        Screen screen(Constants::WINDOW_HEIGHT, Constants::WINDOW_WIDTH, "OpenGL_window");

        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD";
            throw std::runtime_error("Unable to initialize GLAD");
        }

        glEnable(GL_DEPTH_TEST);

        ShaderProgram shaderProgram("vertex_shader.glsl", "fragment_shader.glsl");
        BufferObjects bufferObjects(Shapes::cube_vertices, Shapes::cube_indices);
        Texture texture("dirt.jpg");

        std::array<glm::vec3, 5> cubeCoordinates = {
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(1.0f, -1.0f, 0.0f),
            glm::vec3(-1.0f, -1.0f, 0.0f),
            glm::vec3(2.0f, -1.0f, 0.0f),
            glm::vec3(-2.0f, -1.0f, 0.0f),
        };
        Renderer renderer;
        renderer.addShaderProgram(shaderProgram._shaderProgram);
        for (int i = 0; i < 5; i++) 
            renderer.addRenderData(bufferObjects._VAO, texture._textureID, Shape_Indices::Cube, cubeCoordinates[i]);
        screen.associateRenderer(renderer);
                            //Position                  // Front                      // Up
        Camera camera({ glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        screen.insertCamera(camera);

        screen.run();

        Buffers_Methods::terminateBufferObjects(bufferObjects);
        renderer.terminateShaderProgram();
    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    glfwTerminate();
    return 0;
}
