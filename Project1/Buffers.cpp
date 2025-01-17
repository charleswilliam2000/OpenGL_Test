#include "Buffers.h"

namespace Attributes_Details {
    std::array<VertexAttributes, num_objectAttributes> objectAttributes = {
        VertexAttributes{0, 3, GL_BYTE,  GL_FALSE, sizeof(Vertex), (void*)0 },            //Coordinates
        VertexAttributes{1, 1, GL_BYTE,  GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLbyte)) }, // Packed bits (Normals, uv)
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


    glVertexAttribPointer(attributes[0].index, attributes[0].componentCount, attributes[0].type, attributes[0].normalized, attributes[0].stride, attributes[0].offset);
    glEnableVertexAttribArray(attributes[0].index);

    glVertexAttribIPointer(attributes[1].index, attributes[1].componentCount, attributes[1].type, attributes[1].stride, attributes[1].offset);
    glEnableVertexAttribArray(attributes[1].index);
    
    glBindVertexArray(0);
}
