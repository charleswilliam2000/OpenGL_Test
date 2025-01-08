#include "Buffers.h"

namespace Attributes_Details {
    std::array<VertexAttributes, num_objectAttributes> objectAttributes = {
        VertexAttributes{0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0 },            //Coordinates
        VertexAttributes{1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)) }, // Normals
        VertexAttributes{2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)) } // Textures
    };

   std::array<VertexAttributes, 1> lightSourceAttributes = {
        VertexAttributes{0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 }
    };
}