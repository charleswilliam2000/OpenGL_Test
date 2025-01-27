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

        WorldLighting worldLight
        ( 
            { 
                Shapes::base_cube_vertices, 
                Attributes_Details::voxelFloatAttributes, 
                Shapes::cube_indices 
            }, 
            "light_vertex_shader.glsl", 
            "light_fragment_shader.glsl"
        ); 

        WorldSkybox worldSkybox
        (
            {
                Shapes::base_cube_vertices,
                Attributes_Details::voxelFloatAttributes,
                Shapes::cube_indices
            },
            "skybox_vertex_shader.glsl",
            "skybox_fragment_shader.glsl"
        );

        World world(
            std::move(worldSkybox), 
            std::move(worldLight),
            ShaderProgram{"voxel_vertex_shader.glsl", "voxel_fragment_shader.glsl"}, 
            Texture{"TextureAtlas.jpg"}
        ); world.generateChunks(6);       
                            //Pos                           //Front                     //Up
        Camera camera({ glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        
        game.insertCamera(&camera);
        game.insertWorld(&world);
        game.run();
        glfwTerminate();

    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    return 0;
}
