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

        ShaderProgram worldShader("object_vertex_shader.glsl", "object_fragment_shader.glsl");
        WorldLighting worldLight( 
            { Shapes::base_cube_vertices, Attributes_Details::lightSourceAttributes, Shapes::cube_indices }, 
            "light_vertex_shader.glsl", 
            "light_fragment_shader.glsl");
        worldLight.addPointLight(glm::vec3(18.0f, 18.0f, 18.0f));

        ShaderProgram wireframeShader("wireframe_vertex_shader.glsl", "wireframe_fragment_shader.glsl");

        Texture texture("TextureAtlas.jpg");
        worldShader.setUniform1i("myTextures", texture._textureID);

        World world(&worldLight, worldShader, texture);
        world.generateChunks(6);       
                            //Pos                           //Front                     //Up
        Camera camera({ glm::vec3(20.0f, 20.0f, 20.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        game.insertCamera(&camera);
        game.insertWorld(&world);
        game.run();
        
    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    glfwTerminate();
    return 0;
}
