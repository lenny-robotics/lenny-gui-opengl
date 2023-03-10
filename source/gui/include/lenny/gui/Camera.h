#pragma once

#include <lenny/gui/Utils.h>
#include <lenny/tools/Json.h>

namespace lenny::gui {

class Camera {
public:
    Camera() = default;
    ~Camera() = default;

    //--- Get
    glm::vec3 getPosition() const;
    glm::mat4 getRotation() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    Ray getRayFromScreenCoordinates(double xPos, double yPos, const glm::vec4& viewportParams) const;
    Eigen::Vector3d getGlobalPointFromScreenCoordinates(const Eigen::Vector3d& globalTargetPoint, double xPos, double yPos, const glm::vec4& viewportParams) const;

    //--- Set
    void setAspectRatio(double aspectRatio);

    //--- Interaction
    void updateKeyboardParameters(int key, int action);
    void updateMouseButtonParameters(double xPos, double yPos, int button, int action);

    void processMouseMove(double xPos, double yPos);
    void processMouseScroll(double xOffset, double yOffset);

    //--- Gui
    void drawGui();

    //--- Print, save & load
    void printSettings() const;
    static void to_json(json& j, const Camera& o);
    static void from_json(const json& j, Camera& o);

    //--- Sync
    void sync(const Camera& otherCamera);

private:
    //--- Interaction
    void processLeftMouseMovement(double xOffset, double yOffset);
    void processRightMouseMovement(double xOffset, double yOffset);

public:
    //--- Changeable attributes
    double rotationAboutUpAxis = 0.0;
    double rotationAboutRightAxis = -50.0;
    double distanceToTarget = 5.0;
    Eigen::Vector3d target = Eigen::Vector3d::Zero();

private:
    //--- Changeable attributes
    double aspectRatio = 1.0;

    //--- Fixed attributes
    const glm::vec3 direction = {0.f, 0.f, -1.f};
    const glm::vec3 up = {0.f, 1.f, 0.f};
    const double fieldOfView = 45.0;
    const double zNear = 0.1;
    const double zFar = 1000.0;

    //--- Changeable interaction parameters
    double lastMousePosX = 0.0;
    double lastMousePosY = 0.0;
    bool leftMouseButtonIsPressed = false;
    bool rightMouseButtonIsPressed = false;
    bool leftAltKeyIsPressed = false;
    bool mouseIsDragging = false;

    //--- Fixed interaction parameters
    const double mouseMoveSensitivity = 0.01;
    const double mouseScrollSensitivity = 0.05;
    const double distanceToTargetLimit = 0.1;
    const double targetSensitivity = 0.0005;
};

}  // namespace lenny::gui