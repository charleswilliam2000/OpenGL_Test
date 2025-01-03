#ifndef SHAPE_H
#define SHAPE_H

#include <array>

namespace Shape_Indices {
    constexpr unsigned char Triangle = 3;
    constexpr unsigned char Rectangle = 6;
    constexpr unsigned char Hexagon = 12;
};

namespace Shapes {
    extern std::array<float, 9> triangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Triangle> triangle_indices;

    extern std::array<float, 12> rectangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Rectangle> rectangle_indices;
}

#endif