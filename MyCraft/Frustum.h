#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Camera.h"


struct Plane {
	float_VEC normal = { 0.0f, 0.0f, 0.0f }; // Default normal for each plane
	float distance = 0.0f; // This is the distance of each plane to a given object 

	float distanceToPlane(const float_VEC& point) const {
		return glm::dot(normal, point) - distance;
	}

	float distanceToPoint(const float_VEC& point) const {
		return glm::dot(normal, point) + distance;
	}
};
struct Frustum {
	std::array<Plane, 6> frustumFaces; // left, bottom, far, right, top, near 
	void createFrustumFromCamera(const Camera& camera);
};
struct AABB {
	float_VEC
		min		= { 0.0f, 0.0f, 0.0f },
		max		= { 0.0f, 0.0f, 0.0f };

	AABB(float_VEC min, float_VEC max) 
		: min(min), max(max) {} //Radius assumes a uniform cube size, thus x = y = z

	bool intersects(const AABB& other) const {
		return (
			min.x <= other.max.x && max.x >= other.min.x && 
			min.y <= other.max.y && max.y >= other.min.y && 
			min.z <= other.max.z && max.z >= other.min.z
		);
	}

	bool isOutsideFrustum(const Frustum& cameraFrustum) const {
		for (const auto& frustumFace : cameraFrustum.frustumFaces) {
			bool allOutside = true;
			for (int i = 0; i < 8; i++) {
				float_VEC corner = float_VEC(
					(i & 1) ? max.x : min.x,
					(i & 2) ? max.y : min.y,
					(i & 4) ? max.z : min.z
				);
				if (frustumFace.distanceToPoint(corner) >= 0.0f) {
					allOutside = false;
					break;
				}
			}
			if (allOutside) return true; 
		}
		return false; 
	}
};


#endif // FRUSTUM_H