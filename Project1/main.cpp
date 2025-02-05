#define STB_IMAGE_IMPLEMENTATION

#include "stdafx.h"
#include "Game.h"

int main() {
    try {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            throw std::runtime_error("Unable to initialize GLFW");
        }

        Game game(Constants::WINDOW_HEIGHT, Constants::WINDOW_WIDTH, "OpenGL_window");
        glfwSwapInterval(1); // Enable vsync

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD";
            throw std::runtime_error("Unable to initialize GLAD");
        }

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        World world(
            ShaderProgram{"voxel_vertex_shader.glsl", "voxel_fragment_shader.glsl"}, 
            Texture{"TextureAtlas.jpg"}
        ); 
        world.generateChunks(5, 7);
                            //Pos                           //Front                     //Up
        Camera camera({ glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        
        game.insertCamera(&camera);
        game.insertWorld(&world);
        game.run();
        glfwTerminate();

    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    return 0;
}
