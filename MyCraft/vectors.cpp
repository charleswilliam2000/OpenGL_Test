#include "vectors.h"

uint32_VEC uint32_VEC::getAdjacentCoordinate(const FACES& face) const
{
	switch (face) {
	case FACES::NORTH:		return (z > 0)	? uint32_VEC{ x, y, z - 1 } : *this;
	case FACES::SOUTH:		return (z < 15) ? uint32_VEC{ x, y, z + 1 } : *this;
	case FACES::WEST:		return (x > 0)	? uint32_VEC{ x - 1, y, z } : *this;
	case FACES::EAST:		return (x < 15)	? uint32_VEC{ x + 1, y, z } : *this;
	case FACES::BOTTOM:		return (y > 0)	? uint32_VEC{ x, y - 1, z } : *this;
	case FACES::TOP:		return (y < 15)	? uint32_VEC{ x, y + 1, z } : *this;
	}
	std::cerr << "\nUnable to determine face!";
	throw std::runtime_error("\nFace not recognized!");
}

uint8_VEC uint8_VEC::getAdjacentCoordinate(const FACES& face) const
{
	switch (face) {
	case FACES::NORTH:		return (z > 0) ?	uint8_VEC{ x, y, z - 1 } : *this;
	case FACES::SOUTH:		return (z < 15) ?	uint8_VEC{ x, y, z + 1 } : *this;
	case FACES::WEST:		return (x > 0) ?	uint8_VEC{ x - 1, y, z } : *this;
	case FACES::EAST:		return (x < 15) ?	uint8_VEC{ x + 1, y, z } : *this;
	case FACES::BOTTOM:		return (y > 0) ?	uint8_VEC{ x, y - 1, z } : *this;
	case FACES::TOP:		return (y < 15) ?	uint8_VEC{ x, y + 1, z } : *this;
	}
	std::cerr << "\nUnable to determine face!";
	throw std::runtime_error("\nFace not recognized!");
}
