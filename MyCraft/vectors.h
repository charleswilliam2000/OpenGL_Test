#ifndef VECTORS_H
#define VECTORS_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum class FACES : uint32_t {
	WEST = 0,
	BOTTOM = 1,
	NORTH = 2,

	EAST = 3,
	TOP = 4,
	SOUTH = 5
};


struct uint8_VEC {
	uint8_t x = 0, y = 0, z = 0;
	uint8_VEC() {}
	template<typename _type1, typename _type2, typename _type3>
	constexpr uint8_VEC(_type1 x, _type2 y, _type3 z) : x(static_cast<uint8_t>(x)), y(static_cast<uint8_t>(y)), z(static_cast<uint8_t>(z)) {}
	uint8_VEC getAdjacentCoordinate(const FACES& face) const;

	bool operator==(const uint8_VEC& other) {
		return x == other.x && y == other.y && z == other.z;
	}
};

struct int32_VEC {
	int x = 0, y = 0, z = 0;

	int32_VEC() {}
	template<typename _type1, typename _type2, typename _type3>
	constexpr int32_VEC(_type1 x, _type2 y, _type3 z) : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)) {}

	bool operator==(const int32_VEC& other) {
		return x == other.x && y == other.y && z == other.z;
	}
};

struct uint32_VEC {
	int x = 0, y = 0, z = 0;

	uint32_VEC() {}
	template<typename _type>
	constexpr uint32_VEC(_type x, _type y, _type z) : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)) {};
	uint32_VEC getAdjacentCoordinate(const FACES& face) const;

	bool operator==(const uint32_VEC& other) {
		return x == other.x && y == other.y && z == other.z;
	}
};

using float_VEC = glm::vec3;

#endif