#include "Shape.h"

namespace Shapes {
    std::array<float, 18 * NUM_TRIANGLES> triangle_vertices = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   
       -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   
        0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f     
    };

    std::array<uint32_t, Shape_Indices::Triangle * NUM_TRIANGLES> triangle_indices = {
         0, 1, 2,
    };

    std::array<float, 12 * NUM_RECTANGLES> rectangle_vertices = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };

    std::array<uint32_t, Shape_Indices::Rectangle * NUM_RECTANGLES> rectangle_indices = {
        0, 1, 3,
        1, 2, 3
    };
}