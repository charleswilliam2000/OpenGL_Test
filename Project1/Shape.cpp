#include "Shape.h"

namespace Shapes {
    std::array<float, 24 * NUM_TRIANGLES> triangle_vertices = {
        // positions         // colors              //Textures
        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.5f, 1.0f  
    };

    std::array<uint32_t, Shape_Indices::Triangle * NUM_TRIANGLES> triangle_indices = {
         0, 1, 2,
    };

    std::array<float, 32 * NUM_RECTANGLES> rectangle_vertices = {
        // positions                // colors           //Textures
         0.5f,  0.5f, 0.0f,         1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  
         0.5f, -0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,         0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    std::array<uint32_t, Shape_Indices::Rectangle * NUM_RECTANGLES> rectangle_indices = {
        0, 1, 3,
        1, 2, 3
    };
}