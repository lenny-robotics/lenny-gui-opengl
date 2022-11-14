#pragma once

#include <lenny/tools/Json.h>

#include <glm/glm.hpp>

namespace lenny::gui {

class Light {
public:
    Light() = default;
    ~Light() = default;

    glm::vec3 getPosition() const;
    glm::vec3 getColor() const;
    glm::vec3 getGlow() const;

    void drawGui();

    void printSettings() const;
    static void to_json(json& j, const Light& o);
    static void from_json(const json& j, Light& o);

public:
    Eigen::Vector3d position = Eigen::Vector3d(20.0, 30.0, 20.0);
    Eigen::Vector3d color = Eigen::Vector3d(1.0, 1.0, 1.0);
    double colorIntensity = 1.1;
    double glowIntensity = 0.4;
    float ambientStrength = 0.3f;
    float diffuseStrength = 0.8f;
    float specularStrength = 0.0f;
};

}  // namespace lenny::gui