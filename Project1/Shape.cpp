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

    std::array<float, Vertex_Data::Cube> cube_vertices = {
        // Coordinates         // Textures   // Colors (R, G, B)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   1.0f, 0.0f, 0.0f,  
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0.0f, 1.0f, 0.0f, 
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,  
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 1.0f, 0.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f, 0.0f, 1.0f,  
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   1.0f, 0.5f, 0.5f, 
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0.5f, 0.5f, 1.0f,  

        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.0f, 1.0f, 0.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   1.0f, 1.0f, 0.0f,

         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.0f, 1.0f, 
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.5f, 0.5f,  
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.5f, 0.5f, 1.0f, 

         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.0f, 1.0f, 0.0f,  
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f, 
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f, 1.0f, 0.0f, 

         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   1.0f, 0.0f, 1.0f, 
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f, 1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f, 0.5f, 0.5f, 
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.5f, 0.5f, 1.0f   
    };




    std::array<uint32_t, 6 * 6> cube_indices = {
        // Front face
        0, 1, 3,
        1, 2, 3,

        // Back face
        4, 5, 7,
        5, 6, 7,

        // Left face
        8, 9, 11,
        9, 10, 11,

        // Right face
        12, 13, 15,
        13, 14, 15,

        // Bottom face
        16, 17, 19,
        17, 18, 19,

        // Top face
        20, 21, 23,
        21, 22, 23
    };

}