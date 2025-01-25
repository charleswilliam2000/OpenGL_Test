#include "Frustum.h"

void Frustum::Frustum::createFrustumFromCamera(const Camera& camera, float aspect, float fovY, float zNear, float zFar) {
    auto cameraPos = camera.getVector(CameraVectors::POS);
    auto cameraFront = camera.getVector(CameraVectors::FRONT);
    auto cameraUp = camera.getVector(CameraVectors::UP);
    auto cameraRight = camera.getVector(CameraVectors::RIGHT);

    const float halfVSide = zFar * tanf(fovY * 0.5f);
    const float halfHSide = halfVSide * aspect;
    const float_VEC frontMultFar = zFar * cameraFront;

    float_VEC leftNormal = glm::cross(cameraUp, frontMultFar + cameraRight * halfHSide);
    frustumFaces[0] = { leftNormal, -glm::dot(leftNormal, cameraPos) };

    float_VEC bottomNormal = glm::cross(frontMultFar + cameraUp * halfVSide, cameraRight);
    frustumFaces[1] = { bottomNormal, -glm::dot(bottomNormal, cameraPos) };

    float_VEC farNormal = -cameraFront;
    frustumFaces[2] = { farNormal, -glm::dot(farNormal, cameraPos + frontMultFar) };

    float_VEC rightNormal = glm::cross(frontMultFar - cameraRight * halfHSide, cameraUp);
    frustumFaces[3] = { rightNormal, -glm::dot(rightNormal, cameraPos) };

    float_VEC topNormal = glm::cross(cameraRight, frontMultFar - cameraUp * halfVSide);
    frustumFaces[4] = {topNormal, -glm::dot(topNormal, cameraPos)};

    float_VEC nearNormal = cameraFront;
    frustumFaces[5] = {nearNormal, -glm::dot(nearNormal, cameraPos + zNear * cameraFront)};

}
