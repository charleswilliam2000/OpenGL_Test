#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/glad.h>
#include <vector>
#include <iostream>

#include "Constants.h"

struct VertexAttributes {
    GLuint index{};
    GLint componentCount{};
    GLenum type{};
    GLboolean normalized{};
    GLsizei stride{};
    const void* offset{};
};

struct BufferObjects {
private:
    void generate_vertices_and_elements_buffers();
    void bind_buffers_to_arrays(int array_idx, GLsizeiptr vertices_size, const void* vertices_data, GLsizeiptr vertices_index_size, const void* vertices_index_data);
    void enable_vertex_attributes(int array_idx);
public:
    std::vector<uint32_t> VBOs{};
    std::vector<uint32_t> VAOs{};
    std::vector<uint32_t> EBOs{};

    template <size_t arr1_size, size_t arr2_size>
	BufferObjects(const std::array<float, arr1_size>& vertices, const std::array<uint32_t, arr2_size>& vertices_indices)
    {
            glGenVertexArrays(Constants::NUM_VAO, VAOs.data());

            GLsizeiptr vertices_size = vertices.size() * sizeof(float);
            GLsizeiptr vertices_index_size = vertices_indices.size() * sizeof(uint32_t);

            generate_vertices_and_elements_buffers();

            //Cubes
            bind_buffers_to_arrays(0, vertices_size, vertices.data(), vertices_index_size, vertices_indices.data());
            enable_vertex_attributes(0);

            //Sun
            bind_buffers_to_arrays(1, vertices_size, vertices.data(), vertices_index_size, vertices_indices.data());
            enable_vertex_attributes(1);
    }

    ~BufferObjects() noexcept {
        glDeleteVertexArrays(Constants::NUM_VAO, VAOs.data());
        glDeleteBuffers(Constants::NUM_VBO, VBOs.data());
        glDeleteBuffers(Constants::NUM_EBO, EBOs.data());
    }
};

namespace Buffers_Methods {
	void terminateBufferObjects(BufferObjects& bufferObjects);
}



#endif // BUFFERS_H