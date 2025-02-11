#ifndef SHAPE_H
#define SHAPE_H

#include <array>
#include "vectors.h"

struct PackedVertex {
    GLuint  x : 5;
    GLuint  y : 5;
    GLuint  z : 5;

    GLuint  normals : 3;
    GLuint  uv : 2;
    GLuint  layerIndex : 4;
    GLuint  paddings : 8;

    PackedVertex() : x(0), y(0), z(0), normals(0), uv(0), layerIndex(0), paddings(0) {}
    constexpr PackedVertex(GLuint  x, GLuint  y, GLuint  z, GLuint  normals, GLuint  uv, GLuint  layerIndex)
        : x(x), y(y), z(z), normals(normals), uv(uv), layerIndex(layerIndex), paddings(0) {
    }
};

struct FloatVertex {
    GLfloat x = 0;
    GLfloat y = 0;
    GLfloat z = 0;

    std::array<GLfloat, 3> normals = { 0, 0, 0 };
    std::array<GLfloat, 2> uv = { 0, 0 };

    FloatVertex() : x(0), y(0), z(0), normals({ 0, 0, 0 }), uv({ 0, 0 }) {}
    constexpr FloatVertex(GLfloat x, GLfloat y, GLfloat z, std::array<GLfloat, 3> normal, std::array<GLfloat, 2> uv)
        : x(x), y(y), z(z), normals(normal), uv(uv) {
    }
};

struct PackedFaceData {
    std::array<uint32_t, 6> indices;
    std::array<PackedVertex, 4> vertices;
    FACES face;
};

constexpr std::array<PackedFaceData, 6> PACKED_FACE_DATA = {
    {
        {
            { 0, 2, 1, 0, 3, 2 },
            {
                PackedVertex(0, 0, 1, 0, 3, 0),
                PackedVertex(0, 1, 1, 0, 2, 0),
                PackedVertex(0, 1, 0, 0, 0, 0),
                PackedVertex(0, 0, 0, 0, 1, 0)
            },
            {FACES::WEST},
        },
        {
            {0, 2, 1, 0, 3, 2},
            {
                PackedVertex(0, 0, 1, 1, 1, 0),
                PackedVertex(0, 0, 0, 1, 0, 0),
                PackedVertex(1, 0, 0, 1, 2, 0),
                PackedVertex(1, 0, 1, 1, 3, 0)
            },
            {FACES::BOTTOM},
        },
        {
            {0, 2, 1, 0, 3, 2},
            {
                PackedVertex(0, 0, 0, 2, 1, 0),
                PackedVertex(0, 1, 0, 2, 0, 0),
                PackedVertex(1, 1, 0, 2, 2, 0),
                PackedVertex(1, 0, 0, 2, 3, 0)
            },
            {FACES::NORTH},
        },
        {
            {0, 2, 1, 0, 3, 2},
            {
                PackedVertex(1, 0, 1, 3, 1, 0),
                PackedVertex(1, 0, 0, 3, 3, 0),
                PackedVertex(1, 1, 0, 3, 2, 0),
                PackedVertex(1, 1, 1, 3, 0, 0)
            },
            {FACES::EAST},
        },
        {
            {0, 2, 1, 0, 3, 2},
            {
                PackedVertex(0, 1, 1, 4, 0, 0),
                PackedVertex(1, 1, 1, 4, 2, 0),
                PackedVertex(1, 1, 0, 4, 3, 0),
                PackedVertex(0, 1, 0, 4, 1, 0)
            },
            {FACES::TOP},
        },
        {
            {0, 2, 1, 0, 3, 2},
            {
                PackedVertex(0, 0, 1, 5, 1, 0),
                PackedVertex(1, 0, 1, 5, 3, 0),
                PackedVertex(1, 1, 1, 5, 2, 0),
                PackedVertex(0, 1, 1, 5, 0, 0)
            },
            {FACES::SOUTH},
        }
    }
};

struct FloatFaceData {
    std::array<uint32_t, 6> indices;
    std::array<FloatVertex, 4> vertices;
    FACES face;
};

constexpr std::array<FloatFaceData, 6> FLOAT_FACE_DATA = {
    FloatFaceData {
        {{ 0, 3, 2, 0, 2, 1}},
        {
            FloatVertex{ -0.5f, -0.5f, -0.5f, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
            FloatVertex{  0.5f, -0.5f, -0.5f, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
            FloatVertex{  0.5f,  0.5f, -0.5f, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
            FloatVertex{ -0.5f,  0.5f, -0.5f, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } }
        },
        {FACES::NORTH}
    },
    FloatFaceData {
        {{4, 6, 7, 4, 5, 6}},
        {
            FloatVertex{ -0.5f, -0.5f,  0.5f, { 0.0f, 0.0f,  1.0f }, { 0.0f, 0.0f } },
            FloatVertex{  0.5f, -0.5f,  0.5f, { 0.0f, 0.0f,  1.0f }, { 1.0f, 0.0f } },
            FloatVertex{  0.5f,  0.5f,  0.5f, { 0.0f, 0.0f,  1.0f }, { 1.0f, 1.0f } },
            FloatVertex{ -0.5f,  0.5f,  0.5f, { 0.0f, 0.0f,  1.0f }, { 0.0f, 1.0f } }
        },
        {FACES::SOUTH}
    },
    FloatFaceData {
        {{8, 9, 10, 8, 10, 11}},
        {
            FloatVertex{ -0.5f,  0.5f,  0.5f, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
            FloatVertex{ -0.5f,  0.5f, -0.5f, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
            FloatVertex{ -0.5f, -0.5f, -0.5f, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
            FloatVertex{ -0.5f, -0.5f,  0.5f, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }
        },
        {FACES::WEST}
    },
    FloatFaceData {
        {{12, 14, 13, 12, 15, 14}},
        {
            FloatVertex{  0.5f,  0.5f,  0.5f, { 1.0f, 0.0f,  0.0f }, { 1.0f, 0.0f } },
            FloatVertex{  0.5f,  0.5f, -0.5f, { 1.0f, 0.0f,  0.0f }, { 1.0f, 1.0f } },
            FloatVertex{  0.5f, -0.5f, -0.5f, { 1.0f, 0.0f,  0.0f }, { 0.0f, 1.0f } },
            FloatVertex{  0.5f, -0.5f,  0.5f, { 1.0f, 0.0f,  0.0f }, { 0.0f, 0.0f } }
        },
        {FACES::EAST},
    },
    FloatFaceData {
        {{19, 17, 18, 19, 16, 17}},
        {
            FloatVertex{ -0.5f, -0.5f, -0.5f, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
            FloatVertex{  0.5f, -0.5f, -0.5f, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
            FloatVertex{  0.5f, -0.5f,  0.5f, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
            FloatVertex{ -0.5f, -0.5f,  0.5f, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } }
        },
        {FACES::BOTTOM}
    },
    FloatFaceData {
        {{23, 22, 21, 23, 21, 20}},
        {
            FloatVertex{ -0.5f,  0.5f, -0.5f, { 0.0f, 1.0f,  0.0f }, { 0.0f, 1.0f } },
            FloatVertex{  0.5f,  0.5f, -0.5f, { 0.0f, 1.0f,  0.0f }, { 1.0f, 1.0f } },
            FloatVertex{  0.5f,  0.5f,  0.5f, { 0.0f, 1.0f,  0.0f }, { 1.0f, 0.0f } },
            FloatVertex{ -0.5f,  0.5f,  0.5f, { 0.0f, 1.0f,  0.0f }, { 0.0f, 0.0f } }
        },
        {FACES::TOP}
    }
};

constexpr std::array<FloatVertex, 24> BASE_CUBE_VERTICES = {
    //Coordinates
    FloatVertex{ -0.5f, -0.5f, -0.5f, {}, {} },
    FloatVertex{  0.5f, -0.5f, -0.5f, {}, {} },
    FloatVertex{  0.5f, 0.5f,  -0.5f, {}, {} },
    FloatVertex{ -0.5f, 0.5f,  -0.5f, {}, {} },

    FloatVertex{-0.5f, -0.5f, 0.5f, {}, {} },
    FloatVertex{ 0.5f, -0.5f, 0.5f, {}, {} },
    FloatVertex{ 0.5f,  0.5f, 0.5f, {}, {} },
    FloatVertex{-0.5f,  0.5f, 0.5f, {}, {} },

    FloatVertex{-0.5f,  0.5f,  0.5f, {}, {} },
    FloatVertex{-0.5f,  0.5f, -0.5f, {}, {} },
    FloatVertex{-0.5f, -0.5f, -0.5f, {}, {} },
    FloatVertex{-0.5f, -0.5f,  0.5f, {}, {} },

    FloatVertex{0.5f,  0.5f,   0.5f, {}, {} },
    FloatVertex{0.5f,  0.5f,  -0.5f, {}, {} },
    FloatVertex{0.5f, -0.5f,  -0.5f, {}, {} },
    FloatVertex{0.5f, -0.5f,   0.5f, {}, {} },

    FloatVertex{-0.5f, -0.5f, -0.5f, {}, {} },
    FloatVertex{ 0.5f, -0.5f, -0.5f, {}, {} },
    FloatVertex{ 0.5f, -0.5f,  0.5f, {}, {} },
    FloatVertex{-0.5f, -0.5f,  0.5f, {}, {} },

    FloatVertex{-0.5f, 0.5f, -0.5f, {}, {} },
    FloatVertex{ 0.5f, 0.5f, -0.5f, {}, {} },
    FloatVertex{ 0.5f, 0.5f,  0.5f, {}, {} },
    FloatVertex{-0.5f, 0.5f,  0.5f, {}, {} }
};

constexpr std::array<uint32_t, 36> BASE_CUBE_INDICES = {
    0, 3, 2, 0, 2, 1, // North
    4, 6, 7, 4, 5, 6, // South
    8, 9, 10, 8, 10, 11, // West
    12, 14, 13, 12, 15, 14, // East
    19, 17, 18, 19, 16, 17, // Bottom
    23, 22, 21, 23, 21, 20 // Top
};

#endif SHAPE_H