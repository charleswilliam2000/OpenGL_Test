#include "Shape.h"

namespace Shapes {
    std::array<float, Vertex_Data::Triangle> triangle_vertices = {
        // positions         // colors              //Textures
        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.5f, 1.0f  
    };

    std::array<uint32_t, Shape_Indices::Triangle> triangle_indices = {
         0, 1, 2,
    };

    std::array<float, Vertex_Data::Rectangle> rectangle_vertices = {
        // positions                // colors           //Textures
         0.5f,  0.5f, 0.0f,         1.0f, 0.0f, 0.0f,   1.0f, 1.0f,  
         0.5f, -0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,         0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    std::array<uint32_t, Shape_Indices::Rectangle> rectangle_indices = {
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
    1, 3, 0, 1, 2, 3, // North
    4, 6, 5, 4, 7, 6, // South
    10, 8, 11, 10, 9, 8, // West
    15, 13, 14, 15, 12, 13, // East
    17, 19, 18, 17, 16, 19, // Bottom
    22, 20, 21, 22, 23, 20 // Top
    };


}