#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/glad.h>
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
    void bind_buffers_to_arrays(GLsizeiptr vertices_size, const void* vertices_data, GLsizeiptr vertices_index_size, const void* vertices_index_data);
    void enable_vertex_attributes();
public:
	uint32_t _VBO{};
	uint32_t _VAO{};
	uint32_t _EBO{};

    template <size_t arr1_size, size_t arr2_size>
	BufferObjects(const std::array<float, arr1_size>& vertices, const std::array<uint32_t, arr2_size>& vertices_indices)
    {
            glGenVertexArrays(1, &_VAO);

            GLsizeiptr vertices_size = vertices.size() * sizeof(float);
            GLsizeiptr vertices_index_size = vertices_indices.size() * sizeof(uint32_t);

            generate_vertices_and_elements_buffers();
            bind_buffers_to_arrays(vertices_size, vertices.data(), vertices_index_size, vertices_indices.data());
            enable_vertex_attributes();

            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    ~BufferObjects() noexcept {
        glDeleteVertexArrays(1, &_VAO);
        glDeleteBuffers(1, &_VBO);
        glDeleteBuffers(1, &_EBO);
    }
};

namespace Buffers_Methods {
	void terminateBufferObjects(BufferObjects& bufferObjects);
}



#endif // BUFFERS_H