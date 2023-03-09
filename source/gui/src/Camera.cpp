// clang-format off
#include <glad/glad.h> //Glad needs to be included before glfw!
#include <GLFW/glfw3.h>
// clang-format on
#include <lenny/gui/Camera.h>
#include <lenny/gui/ImGui.h>
#include <lenny/tools/Logger.h>

#include <glm/gtc/matrix_transform.hpp>

namespace lenny::gui {

glm::vec3 Camera::getPosition() const {
    return utils::toGLM(target) + glm::vec3(getRotation() * glm::vec4(direction, 1)) * (float)-distanceToTarget;
}

glm::mat4 Camera::getRotation() const {
    glm::mat4 rot(1.f);
    rot = glm::rotate(rot, (float)rotationAboutUpAxis, up);
    rot = glm::rotate(rot, (float)rotationAboutRightAxis, glm::cross(up, direction));
    return rot;
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 worldUp = getRotation() * glm::vec4(up, 1);
    return glm::lookAt(getPosition(), utils::toGLM(target), worldUp);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fieldOfView), aspectRatio, zNear, zFar);
}

Ray Camera::getRayFromScreenCoordinates(double xPos, double yPos, const glm::vec4& viewportParams) const {
    const glm::vec3 p1 = glm::unProject(glm::vec3(xPos, viewportParams[3] - yPos - 1, 0), getViewMatrix(), getProjectionMatrix(), viewportParams);
    const glm::vec3 p2 = glm::unProject(glm::vec3(xPos, viewportParams[3] - yPos - 1, 1), getViewMatrix(), getProjectionMatrix(), viewportParams);

    return {utils::toEigen(p1), utils::toEigen(p2 - p1).normalized()};  //[origin, direction]
}

Eigen::Vector3d Camera::getGlobalPointFromScreenCoordinates(const Eigen::Vector3d& globalTargetPoint, double xPos, double yPos, const glm::vec4& viewportParams) const {
    const Ray ray = getRayFromScreenCoordinates(xPos, yPos, viewportParams);
    struct Plane {
        Eigen::Vector3d point, normal;
    };
    const Plane plane = {globalTargetPoint, (gui::utils::toEigen(getPosition()) - target).normalized()};
    if (fabs(ray.direction.dot(plane.normal)) < 1e-8)
        return ray.origin;
    const double t = (plane.point - ray.origin).dot(plane.normal) / ray.direction.dot(plane.normal);
    if (t < 0)
        return ray.origin;
    return ray.origin + ray.direction * t;
}

void Camera::setAspectRatio(double aspectRatio) {
    this->aspectRatio = aspectRatio;
}

void Camera::updateKeyboardParameters(int key, int action) {
    leftAltKeyIsPressed = (key == GLFW_KEY_LEFT_ALT) && (action == GLFW_PRESS);
}

void Camera::updateMouseButtonParameters(double xPos, double yPos, int button, int action) {
    lastMousePosX = xPos;
    lastMousePosY = yPos;

    mouseIsDragging = (action == GLFW_PRESS);

    if (mouseIsDragging) {
        leftMouseButtonIsPressed = (button == GLFW_MOUSE_BUTTON_LEFT);
        rightMouseButtonIsPressed = (button == GLFW_MOUSE_BUTTON_RIGHT);
    }
}

void Camera::processMouseMove(double xPos, double yPos) {
    const double mouseMoveX = lastMousePosX - xPos;
    const double mouseMoveY = -lastMousePosY + yPos;
    lastMousePosX = xPos;
    lastMousePosY = yPos;

    if (mouseIsDragging) {
        if (leftMouseButtonIsPressed && !leftAltKeyIsPressed)
            processLeftMouseMovement(mouseMoveX, mouseMoveY);

        if (rightMouseButtonIsPressed || (leftMouseButtonIsPressed && leftAltKeyIsPressed))
            processRightMouseMovement(mouseMoveX, mouseMoveY);
    }
}

void Camera::processMouseScroll(double xOffset, double yOffset) {
    distanceToTarget *= 1.0 - yOffset * mouseScrollSensitivity;
    if (distanceToTarget < distanceToTargetLimit)
        distanceToTarget = distanceToTargetLimit;
}

void Camera::processLeftMouseMovement(double xOffset, double yOffset) {
    rotationAboutUpAxis += xOffset * mouseMoveSensitivity;
    rotationAboutRightAxis += yOffset * mouseMoveSensitivity;
}

void Camera::processRightMouseMovement(double xOffset, double yOffset) {
    glm::vec3 m = {xOffset, yOffset, 0.0};
    glm::mat4 rot = getViewMatrix();
    glm::vec3 target_tmp = utils::toGLM(this->target);
    target_tmp += (float)targetSensitivity * glm::vec3(glm::inverse(rot) * glm::vec4(m, 0.0)) * (float)distanceToTarget;
    this->target = utils::toEigen(target_tmp);
}

void Camera::drawGui() {
    if (ImGui::TreeNode("Camera")) {
        if (ImGui::Button("Print Settings"))
            printSettings();

        ImGui::TreePop();
    }
}

void Camera::printSettings() const {
    using tools::Logger;

    LENNY_LOG_PRINT(Logger::DEFAULT, "--- CAMERA SETTINGS ---\n");
    LENNY_LOG_PRINT(Logger::DEFAULT, "camera.rotationAboutUpAxis = %lf;\n", rotationAboutUpAxis);
    LENNY_LOG_PRINT(Logger::DEFAULT, "camera.rotationAboutRightAxis = %lf;\n", rotationAboutRightAxis);
    LENNY_LOG_PRINT(Logger::DEFAULT, "camera.distanceToTarget = %lf;\n", distanceToTarget);
    LENNY_LOG_PRINT(Logger::DEFAULT, "camera.target = Eigen::Vector3d(%lf, %lf, %lf);\n", target.x(), target.y(), target.z());
    LENNY_LOG_PRINT(Logger::DEFAULT, "------------------------\n");
}

void Camera::to_json(json& j, const Camera& o) {
    TO_JSON(o, rotationAboutUpAxis)
    TO_JSON(o, rotationAboutRightAxis)
    TO_JSON(o, distanceToTarget)
    TO_JSON(o, target)
}

void Camera::from_json(const json& j, Camera& o) {
    FROM_JSON(o, rotationAboutUpAxis)
    FROM_JSON(o, rotationAboutRightAxis)
    FROM_JSON(o, distanceToTarget)
    FROM_JSON(o, target)
}

void Camera::sync(const Camera& otherCamera){
    this->rotationAboutUpAxis = otherCamera.rotationAboutUpAxis;
    this->rotationAboutRightAxis = otherCamera.rotationAboutRightAxis;
    this->distanceToTarget = otherCamera.distanceToTarget;
    this->target = otherCamera.target;
}

}  // namespace lenny::gui