#include "vectors.h"

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

uint8_VEC uint8_VEC::getAdjacentCoordinate(const Faces& face) const
{
	switch (face) {
	case Faces::NORTH:		return (z > 0) ?	uint8_VEC{ x, y, z - 1 } : *this;
	case Faces::SOUTH:		return (z < 15) ?	uint8_VEC{ x, y, z + 1 } : *this;
	case Faces::WEST:		return (x > 0) ?	uint8_VEC{ x - 1, y, z } : *this;
	case Faces::EAST:		return (x < 15) ?	uint8_VEC{ x + 1, y, z } : *this;
	case Faces::BOTTOM:		return (y > 0) ?	uint8_VEC{ x, y - 1, z } : *this;
	case Faces::TOP:		return (y < 15) ?	uint8_VEC{ x, y + 1, z } : *this;
	}
	std::cerr << "\nUnable to determine face!";
	throw std::runtime_error("\nFace not recognized!");
}
