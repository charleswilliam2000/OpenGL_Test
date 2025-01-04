#ifndef SHAPE_H
#define SHAPE_H

#include <array>

constexpr unsigned char NUM_TRIANGLES = 1;
constexpr unsigned char NUM_RECTANGLES = 1;

namespace Shape_Indices {
    constexpr unsigned char Triangle = 3;
    constexpr unsigned char Rectangle = 6;
    constexpr unsigned char Hexagon = 12;
};

namespace Shapes {
    extern std::array<float, 24 * NUM_TRIANGLES> triangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Triangle * NUM_TRIANGLES> triangle_indices;

    extern std::array<float, 32 * NUM_RECTANGLES> rectangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Rectangle * NUM_RECTANGLES> rectangle_indices;
}

#endif