#ifndef VECTORS_H
#define VECTORS_H

#include <iostream>

enum class Faces : uint8_t {
	NORTH = 0,
	SOUTH = 1,
	WEST = 2,
	EAST = 3,
	BOTTOM = 4,
	TOP = 5
};

struct uint16_VEC {
	uint16_t x, y, z;
	uint16_VEC getAdjacentCoordinate(const Faces& face) const;
};

struct uint32_VEC {
	int x = 0, y = 0, z = 0;

	uint32_VEC() {}
	template<typename _type>
	uint32_VEC(_type x, _type y, _type z) : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)) {};
	uint32_VEC getAdjacentCoordinate(const Faces& face) const;

	bool operator==(const uint32_VEC& other) {
		return x == other.x && y == other.y && z == other.z;
	}
};

struct float_VEC {
	float x = 0.0f, y = 0.0f, z = 0.0f;

	float_VEC() {}
	template<typename _type>
	float_VEC(_type x, _type y, _type z) : x(static_cast<float>(x)), y(static_cast<float>(y)), z(static_cast<float>(z)) {};
	float_VEC getAdjacentCoordinate(const Faces& face) const;
};




#endif