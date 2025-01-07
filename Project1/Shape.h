#ifndef SHAPE_H
#define SHAPE_H

#include <array>
#include "Constants.h"

namespace Vertex_Data {
    constexpr unsigned char Triangle = 24;
    constexpr unsigned char Rectangle = 32;
    constexpr unsigned char Cube = 120;
}

namespace Shape_Indices {
    constexpr unsigned char Triangle = 3;
    constexpr unsigned char Rectangle = 6;
    constexpr unsigned char Hexagon = 12;
    constexpr unsigned char Cube = 36;
};

namespace Shapes {
    extern std::array<float, Vertex_Data::Triangle * Constants::NUM_TRIANGLES> triangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Triangle * Constants::NUM_TRIANGLES> triangle_indices;

    extern std::array<float, Vertex_Data::Rectangle * Constants::NUM_RECTANGLES> rectangle_vertices;
    extern std::array<uint32_t, Shape_Indices::Rectangle * Constants::NUM_RECTANGLES> rectangle_indices;

    extern std::array<float, Vertex_Data::Cube> cube_vertices;
    extern std::array<uint32_t, Shape_Indices::Cube> cube_indices;
}

#endif // SHAPE_H