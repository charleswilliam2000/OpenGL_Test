#include "Buffers.h"

namespace Attributes_Details {
    std::array<VertexAttributes, 3> objectAttributes = {
        VertexAttributes{0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0 },            //Coordinates
        VertexAttributes{1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)) }, // Textures
        VertexAttributes{2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)) } // Normals
    };

   std::array<VertexAttributes, 1> lightSourceAttributes = {
        VertexAttributes{0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0 }
    };
}