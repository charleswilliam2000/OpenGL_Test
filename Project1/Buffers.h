#ifndef BUFFERS_H
#define BUFFERS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

struct BufferObjects {
	uint32_t _VBO{};
	uint32_t _VAO{};
	uint32_t _EBO{};

    template <size_t arr1_size, size_t arr2_size>
	BufferObjects(const std::array<float, arr1_size>& vertices, const std::array<uint32_t, arr2_size>& vertices_indices)
    {
            glGenVertexArrays(1, &_VAO);

            glGenBuffers(1, &_VBO);
            glGenBuffers(1, &_EBO);

            glBindVertexArray(_VAO);

            glBindBuffer(GL_ARRAY_BUFFER, _VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices_indices.size() * sizeof(int), vertices_indices.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindVertexArray(0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
};

namespace Buffers_Methods {
	void terminateBufferObjects(BufferObjects& bufferObjects);
}



#endif // BUFFERS_H