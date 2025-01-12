#include "vectors.h"

uint16_VEC uint16_VEC::getAdjacentCoordinate(const Faces& face) const {
	switch (face) {
	case Faces::NORTH:		return { x, y, static_cast<uint16_t>((z - 1 > 0) ? z : 0) };
	case Faces::SOUTH:		return { x, y, static_cast<uint16_t>(z + 1) };
	case Faces::EAST:		return { static_cast<uint16_t>(x + 1), y, z };
	case Faces::WEST:		return { static_cast<uint16_t>((x - 1 > 0) ? x : 0), y, z };
	case Faces::TOP:		return { x, static_cast<uint16_t>(y + 1), z };
	case Faces::BOTTOM:		return { x, static_cast<uint16_t>((y - 1 > 0) ? y : 0), z };

	}
	std::cerr << "\nUnable to determine face!";
	throw std::runtime_error("\nFace not recognized!");
}

uint32_VEC uint32_VEC::getAdjacentCoordinate(const Faces& face) const
{
	switch (face) {
	case Faces::NORTH:		return (z > 0)	? uint32_VEC{ x, y, z - 1 } : *this;
	case Faces::SOUTH:		return (z < 15) ? uint32_VEC{ x, y, z + 1 } : *this;
	case Faces::WEST:		return (x > 0)	? uint32_VEC{ x - 1, y, z } : *this;
	case Faces::EAST:		return (x < 15)	? uint32_VEC{ x + 1, y, z } : *this;
	case Faces::BOTTOM:		return (y > 0)	? uint32_VEC{ x, y - 1, z } : *this;
	case Faces::TOP:		return (y < 15)	? uint32_VEC{ x, y + 1, z } : *this;
	}
	std::cerr << "\nUnable to determine face!";
	throw std::runtime_error("\nFace not recognized!");
}


float_VEC float_VEC::getAdjacentCoordinate(const Faces& face) const
{
	switch (face) {
	case Faces::NORTH:		return { x, y, z - 1.0f };
	case Faces::SOUTH:		return { x, y, z + 1.0f };
	case Faces::EAST:		return { x + 1.0f, y, z };
	case Faces::WEST:		return { x - 1.0f, y, z };
	case Faces::TOP:		return { x, y + 1.0f, z };
	case Faces::BOTTOM:		return { x, y - 1.0f, z };

	}
	std::cerr << "\nUnable to determine face!";
	throw std::runtime_error("\nFace not recognized!");
}
