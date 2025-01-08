#include "Shape.h"

namespace Shapes {
    std::array<float, Vertex_Data::Triangle * Constants::NUM_TRIANGLES> triangle_vertices = {
        // positions         // colors              //Textures
        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.5f, 1.0f  
    };

    std::array<uint32_t, Shape_Indices::Triangle * Constants::NUM_TRIANGLES> triangle_indices = {
         0, 1, 2,
    };

    std::array<float, Vertex_Data::Rectangle * Constants::NUM_RECTANGLES> rectangle_vertices = {
        // positions                // colors           //Textures
         0.5f,  0.5f, 0.0f,         1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  
         0.5f, -0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,         0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    std::array<uint32_t, Shape_Indices::Rectangle * Constants::NUM_RECTANGLES> rectangle_indices = {
        0, 1, 3,
        1, 2, 3
    };

    std::array<float, Vertex_Data::Textured_Cube> textured_cube_vertices = {
        // Coordinates         // Normals               //Textures   
        -0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,    0.0f, 0.0f, -1.0f,      1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,    0.0f, 0.0f, -1.0f,      0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,       1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,       1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,       0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,       1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,   -1.0f, 0.0f, 0.0f,       0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,   -1.0f, 0.0f, 0.0f,       0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,   1.0f, 0.0f, 0.0f,        1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   1.0f, 0.0f, 0.0f,        1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,        0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 0.0f,        0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,       0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,   0.0f, -1.0f, 0.0f,       1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,       1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, -1.0f, 0.0f,       0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,        0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f,        1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,        1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 0.0f,        0.0f, 0.0f,
    };

    std::array<float, Vertex_Data::Base_Cube> base_cube_vertices = {
        //Coordinates
        -0.5, -0.5f, -0.5f,
         0.5, -0.5f, -0.5f,
         0.5, 0.5f,  -0.5f,
        -0.5, 0.5f,  -0.5f,

        -0.5, -0.5f, 0.5f,
         0.5, -0.5f, 0.5f,
         0.5,  0.5f, 0.5f,
        -0.5,  0.5f, 0.5f,

        -0.5,  0.5f,  0.5f,
        -0.5,  0.5f, -0.5f,
        -0.5, -0.5f, -0.5f,
        -0.5, -0.5f,  0.5f,

        0.5,  0.5f,   0.5f,
        0.5,  0.5f,  -0.5f,
        0.5, -0.5f,  -0.5f,
        0.5, -0.5f,   0.5f,

        -0.5, -0.5f, -0.5f,
         0.5, -0.5f, -0.5f,
         0.5, -0.5f,  0.5f,
        -0.5, -0.5f,  0.5f,

        -0.5, 0.5f, -0.5f,
         0.5, 0.5f, -0.5f,
         0.5, 0.5f,  0.5f,
        -0.5, 0.5f,  0.5f,
    };

    std::array<uint32_t, Shape_Indices::Cube> cube_indices = {
        0, 1, 3,
        1, 2, 3,

        4, 5, 7,
        5, 6, 7,

        8, 9, 11,
        9, 10, 11,

        12, 13, 15,
        13, 14, 15,

        16, 17, 19,
        17, 18, 19,

        20, 21, 23,
        21, 22, 23
    };

}