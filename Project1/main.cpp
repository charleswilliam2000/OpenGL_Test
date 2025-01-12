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
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        ShaderProgram objectShader("object_vertex_shader.glsl", "object_fragment_shader.glsl");
        ShaderProgram lightShader("light_vertex_shader.glsl", "light_fragment_shader.glsl");
        ShaderProgram wireframeShader("wireframe_vertex_shader.glsl", "wireframe_fragment_shader.glsl");

        Texture texture("dirt.jpg");
        Shader_Methods::setUniform1i(objectShader._shaderProgram, "myTextures", texture._textureID);
       

        const int offset = 8;  // Range from -16.0f to 16.0f

        std::vector<float_VEC> cubeCoordinates; cubeCoordinates.reserve(16 * 16 * 16);
        for (int x = -offset; x < offset; x++) {
            for (int y = -offset; y < offset; y++) {
                for (int z = -offset; z < offset; z++) {
                    cubeCoordinates.emplace_back(x, y, z);
                }
            }
        }
        const size_t num_cubes = cubeCoordinates.size();
        std::vector<BufferObjects> cubes; cubes.reserve(num_cubes);
        int index = 0;

        Chunk chunk; auto& blocks = *(chunk.blocks);
        Chunk_Methods::generateChunk(blocks, cubeCoordinates, cubes);
        BufferObjects lightSource(Shapes::base_cube_vertices, Attributes_Details::lightSourceAttributes, Shapes::cube_indices);

        Renderer renderer;
        renderer.addObjectShader(objectShader._shaderProgram);
        for (int i = 0; i < num_cubes; i++) {
            renderer.addObjectData({ cubes[i].VAO, texture._textureID, cubes[i].object_indices, cubeCoordinates[i]});
        }

        renderer.addLightSourceShader(lightShader._shaderProgram);
        renderer.addLightSourceData({ lightSource.VAO, 0, Shape_Indices::Cube, {33.0f, 33.0f, 33.0f} });
        renderer.addWireframeShader(wireframeShader._shaderProgram);

        screen.associateRenderer(renderer);

        Camera camera({ glm::vec3(17.0f, 17.0f, 17.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f) });
        screen.insertCamera(camera);
        screen.run();

        renderer.terminateShaderPrograms();
        
    }
    catch (const std::runtime_error& e) { std::cout << e.what(); }
    glfwTerminate();
    return 0;
}
