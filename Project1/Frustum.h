#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Camera.h"

namespace Frustum {

	struct Plane {
		float_VEC normal = { 0.0f, 0.0f, 0.0f }; // Default normal for each plane
		float distance = 0.0f; // This is the distance of each plane to a given object 

		float distanceToPoint(const float_VEC& point) const {
			return glm::dot(normal, point) + distance;
		}
	};
	struct Frustum {
		std::array<Plane, 6> frustumFaces; // left, bottom, far, right, top, near 
		void createFrustumFromCamera(const Camera& camera, float aspect, float fovY, float zNear, float zFar);
	};
	struct AABB {
		float_VEC
			center	= { 0.0f, 0.0f, 0.0f },
			min		= { 0.0f, 0.0f, 0.0f },
			max		= { 0.0f, 0.0f, 0.0f };
		float radius = 0.0f;

		AABB(float_VEC min, float_VEC max) 
			: min(min), max(max), center((min + max) * 0.5f), radius((max.x - min.x) * 0.5f) {} //Radius assumes a uniform cube size, thus x = y = z

		bool intersects(const AABB& other) const {
			return (
				min.x <= other.max.x && max.x >= other.min.x && 
				min.y <= other.max.y && max.y >= other.min.y && 
				min.z <= other.max.z && max.z >= other.min.z
			);
		}

		bool isOutsideFrustum(const Frustum& cameraFrustum) const {
			for (const auto& frustumFace : cameraFrustum.frustumFaces) {
				float signedDistance = frustumFace.distanceToPoint(center);
				if (signedDistance + radius > 0.0f)
					return true;
			}
			return false;
		}
	};
}

#endif // FRUSTUM_H