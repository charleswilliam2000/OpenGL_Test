#define STB_IMAGE_IMPLEMENTATION

#include "stdafx.h"
#include "Game.h"

int main() {
    try {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            throw std::runtime_error("Unable to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        Game game(CONSTANTS::WINDOW_WIDTH, CONSTANTS::WINDOW_HEIGHT, "OpenGL_window");
        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD";
            throw std::runtime_error("Unable to initialize GLAD");
        }

        glEnable(GL_FRAMEBUFFER_SRGB);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        World world(7, 7);                            //Pos                           //Front                     //Up
        Camera camera({ glm::vec3(20.0f, 50.0f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        
        game.insertCamera(&camera);
        game.insertWorld(&world);
        game.run();
        glfwTerminate();

    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    return 0;
}
