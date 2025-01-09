#ifndef SHAPE_H
#define SHAPE_H

#include <array>

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

namespace Shapes {
    extern std::array<float, Vertex_Data::Triangle> triangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Triangle> triangle_indices;

    extern std::array<float, Vertex_Data::Rectangle> rectangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Rectangle> rectangle_indices;

    extern std::array<float, Vertex_Data::Textured_Cube> textured_cube_vertices;
    extern std::array<float, Vertex_Data::Base_Cube> base_cube_vertices;
    extern std::array<uint32_t, Shape_Indices::Cube> cube_indices;
}

#endif // SHAPE_H