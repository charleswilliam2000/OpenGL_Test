#include "Buffers.h"

namespace Attributes_Details {
    std::array<VertexAttributes, num_objectAttributes> objectAttributes = {
        VertexAttributes{0, 3, GL_BYTE,  GL_FALSE, 4 * sizeof(uint8_t), (void*)0 },            //Coordinates
        VertexAttributes{1, 1, GL_BYTE,  GL_FALSE, 4 * sizeof(uint8_t), (void*)(3 * sizeof(uint8_t)) }, // Packed bits
    };

   std::array<VertexAttributes, 1> lightSourceAttributes = {
        VertexAttributes{0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 }
    };
}

BufferObjects::BufferObjects(
    const std::vector<Vertex>& vertex, 
    const std::array<VertexAttributes, Attributes_Details::num_objectAttributes>& attributes, 
    const std::vector<uint32_t>& indices)
    : object_indices(static_cast<int>(indices.size()))
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), vertex.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);


    for (const auto& attr : attributes) {
        glVertexAttribPointer(attr.index, attr.componentCount, attr.type, attr.normalized, attr.stride, attr.offset);
        glEnableVertexAttribArray(attr.index);
    }
    glBindVertexArray(0);
}
