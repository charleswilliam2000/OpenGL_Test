#include "Frustum.h"

void Frustum::createFrustumFromCamera(const Camera& camera) {
    auto cameraPos = camera.getVector(CameraVectors::POS);
    auto cameraFront = camera.getVector(CameraVectors::FRONT);
    auto cameraUp = camera.getVector(CameraVectors::UP);
    auto cameraRight = camera.getVector(CameraVectors::RIGHT);

    const float halfVSide = camera.zFar * tanf(camera.fovY * 0.5f);
    const float halfHSide = halfVSide * camera.aspect;
    const float_VEC frontMultFar = camera.zFar * cameraFront;

    float_VEC leftNormal = glm::cross(cameraUp, frontMultFar + cameraRight * halfHSide);
    leftNormal = glm::normalize(leftNormal);
    frustumFaces[0] = { leftNormal, -glm::dot(leftNormal, cameraPos) };

    float_VEC bottomNormal = glm::cross(frontMultFar + cameraUp * halfVSide, cameraRight);
    bottomNormal = glm::normalize(bottomNormal);
    frustumFaces[1] = { bottomNormal, -glm::dot(bottomNormal, cameraPos) };

    float_VEC farNormal = -cameraFront;
    farNormal = glm::normalize(farNormal);
    frustumFaces[2] = { farNormal, -glm::dot(farNormal, cameraPos + frontMultFar) };

    float_VEC rightNormal = glm::cross(frontMultFar - cameraRight * halfHSide, cameraUp);
    rightNormal = glm::normalize(rightNormal);
    frustumFaces[3] = { rightNormal, -glm::dot(rightNormal, cameraPos) };

    float_VEC topNormal = glm::cross(cameraRight, frontMultFar - cameraUp * halfVSide);
    topNormal = glm::normalize(topNormal);
    frustumFaces[4] = { topNormal, -glm::dot(topNormal, cameraPos)};

    float_VEC nearNormal = cameraFront;
    nearNormal = glm::normalize(nearNormal);
    frustumFaces[5] = { nearNormal, -glm::dot(nearNormal, cameraPos + camera.zNear * cameraFront)};

}
