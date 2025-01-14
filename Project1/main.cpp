#define STB_IMAGE_IMPLEMENTATION
#include "stdafx.h"
#include "Chunk.h"
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
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        ShaderProgram objectShader("object_vertex_shader.glsl", "object_fragment_shader.glsl");
        ShaderProgram lightShader("light_vertex_shader.glsl", "light_fragment_shader.glsl");
        ShaderProgram wireframeShader("wireframe_vertex_shader.glsl", "wireframe_fragment_shader.glsl");

        Texture texture("dirt.jpg");
        Shader_Methods::setUniform1i(objectShader._shaderProgram, "myTextures", texture._textureID);
       
        constexpr int offset = 16;

        std::vector<uint8_VEC> block_coordinates; block_coordinates.reserve(offset * offset * offset);

        for (int x = 0; x < offset; x++) {
            for (int y = 0; y < offset; y++) {
                for (int z = 0; z < offset; z++) {
                    block_coordinates.emplace_back(x, y, z);
                }
            }
        }
        Chunk chunk(block_coordinates);
        BufferObjects lightSource(Shapes::base_cube_vertices, Attributes_Details::lightSourceAttributes, Shapes::cube_indices);

        Renderer renderer;
        renderer.addObjectShader(objectShader._shaderProgram);
        renderer.addObjectData({ chunk.chunkData.VAO, texture._textureID, chunk.chunkData.object_indices, {0.0f, 0.0f, 0.0f} });
        

        renderer.addLightSourceShader(lightShader._shaderProgram);
        renderer.addLightSourceData({ lightSource.VAO, 0, Shape_Indices::Cube, {17.0f, 17.0f, 17.0f} });
        renderer.addWireframeShader(wireframeShader._shaderProgram);

        screen.associateRenderer(renderer);
                            //Pos                           //Front                     //Up
        Camera camera({ glm::vec3(17.0f, 17.0f, 17.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        screen.insertCamera(camera);
        screen.run();

        renderer.terminateShaderPrograms();
        
    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    glfwTerminate();
    return 0;
}
