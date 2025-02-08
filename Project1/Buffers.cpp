#include "Buffers.h"


DrawableBufferObjects::DrawableBufferObjects(
    const std::vector<Vertex>& vertex, 
    const std::array<VertexAttributes, 1>& attributes, 
    const std::vector<uint32_t>& indices)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(Vertex), vertex.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    for (const auto& attrib : attributes) {
        glVertexAttribIPointer(attrib.index, attrib.componentCount, attrib.type, attrib.stride, attrib.offset);
        glEnableVertexAttribArray(attrib.index);
    }
  
    glBindVertexArray(0);
}
