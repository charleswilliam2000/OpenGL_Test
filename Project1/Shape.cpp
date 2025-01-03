#include "Shape.h"

namespace Shapes {
    std::array<float, 9> triangle_vertices = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    std::array<uint32_t, Shape_Indices::Triangle> triangle_indices = {
         0, 1, 2
    };

    std::array<float, 12> rectangle_vertices = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    std::array<uint32_t, Shape_Indices::Rectangle> rectangle_indices = {
        0, 1, 3,
        1, 2, 3
    };
}