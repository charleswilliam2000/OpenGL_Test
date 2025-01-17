#ifndef SHAPE_H
#define SHAPE_H

#include <array>
#include <vector>

namespace Vertex_Data {
    constexpr unsigned char Triangle = 24;
    constexpr unsigned char Rectangle = 32;
    constexpr unsigned char Base_Cube = 72;
    constexpr unsigned char Textured_Cube = 192;
}

namespace Shape_Indices {
    constexpr unsigned char Triangle = 3;
    constexpr unsigned char Rectangle = 6;
    constexpr unsigned char Hexagon = 12;
    constexpr unsigned char Cube = 36;
};

struct alignas(4) Vertex {
    std::array<GLbyte, 3> coordinates = {0, 0, 0};
    GLbyte vertex_data = 0; // 4 for normals, and 2 for uv. The rest is unusued bits

    Vertex() {}
    constexpr Vertex(std::array<GLbyte, 3> coordinates, uint8_t vertex_data) : coordinates(coordinates), vertex_data(vertex_data) {}
};


namespace Shapes {
    constexpr std::array<float, Vertex_Data::Triangle> triangle_vertices = {
        // positions         // colors              //Textures
        0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
       -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        0.0f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,   0.5f, 1.0f
    };

    constexpr std::array<uint32_t, Shape_Indices::Triangle> triangle_indices = {
         0, 1, 2,
    };

    constexpr std::array<float, Vertex_Data::Rectangle> rectangle_vertices = {
        // positions                // colors           //Textures
         0.5f,  0.5f, 0.0f,         1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,         0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,         0.0f, 1.0f, 0.0f,   0.0f, 1.0f
    };

    constexpr std::array<uint32_t, Shape_Indices::Rectangle> rectangle_indices = {
        0, 1, 3,
        1, 2, 3
    };

    constexpr std::array<float, Vertex_Data::Textured_Cube> textured_cube_vertices = {
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
    constexpr std::array<float, Vertex_Data::Base_Cube> base_cube_vertices = {
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
    constexpr std::array<uint32_t, Shape_Indices::Cube> cube_indices = {
        1, 3, 0, 1, 2, 3, // North
        4, 6, 5, 4, 7, 6, // South
        10, 8, 11, 10, 9, 8, // West
        15, 13, 14, 15, 12, 13, // East
        17, 19, 18, 17, 16, 19, // Bottom
        22, 20, 21, 22, 23, 20 // Top
    };
}

#endif // SHAPE_H