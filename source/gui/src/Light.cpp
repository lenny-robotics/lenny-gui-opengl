#include <lenny/gui/ImGui.h>
#include <lenny/gui/Light.h>
#include <lenny/gui/Utils.h>

namespace lenny::gui {

glm::vec3 Light::getPosition() const {
    return utils::toGLM(position);
}

glm::vec3 Light::getColor() const {
    return utils::toGLM(colorIntensity * color);
}

glm::vec3 Light::getGlow() const {
    return utils::toGLM(glowIntensity * Eigen::Vector3d::Ones());
}

void Light::drawGui() {
    if (ImGui::TreeNode("Light")) {
        ImGui::SliderVectorD<3>("Position", position, -100.0 * Eigen::Vector3d::Ones(), 100.0 * Eigen::Vector3d::Ones());
        ImGui::ColorPicker3("Color", color);
        ImGui::SliderDouble("Color Intensity", &colorIntensity, 0.0, 2.0);
        ImGui::SliderDouble("Glow Intensity", &glowIntensity, 0.0, 10.0);
        ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.f, 1.f);
        ImGui::SliderFloat("Diffuse Strength", &diffuseStrength, 0.f, 1.f);
        ImGui::SliderFloat("Specular Strength", &specularStrength, 0.f, 1.f);

        if (ImGui::Button("Print Settings"))
            printSettings();

        ImGui::TreePop();
    }
}

void Light::printSettings() const {
    using tools::Logger;
    LENNY_LOG_PRINT(Logger::DEFAULT, "--- LIGHT SETTINGS ---\n");
    LENNY_LOG_PRINT(Logger::DEFAULT, "light.position = Eigen::Vector3d(%lf, %lf, %lf);\n", position.x(), position.y(), position.z());
    LENNY_LOG_PRINT(Logger::DEFAULT, "light.color = Eigen::Vector3d(%lf, %lf, %lf);\n", color.x(), color.y(), color.z());
    LENNY_LOG_PRINT(Logger::DEFAULT, "light.colorIntensity = %lf;\n", colorIntensity);
    LENNY_LOG_PRINT(Logger::DEFAULT, "light.glowIntensity = %lf;\n", glowIntensity);
    LENNY_LOG_PRINT(Logger::DEFAULT, "light.ambientStrength = %lf;\n", ambientStrength);
    LENNY_LOG_PRINT(Logger::DEFAULT, "light.diffuseStrength = %lf;\n", diffuseStrength);
    LENNY_LOG_PRINT(Logger::DEFAULT, "light.specularStrength = %lf;\n", specularStrength);
    LENNY_LOG_PRINT(Logger::DEFAULT, "------------------------\n");
}

void Light::to_json(json& j, const Light& o) {
    TO_JSON(o, position)
    TO_JSON(o, color)
    TO_JSON(o, colorIntensity)
    TO_JSON(o, glowIntensity)
    TO_JSON(o, ambientStrength)
    TO_JSON(o, diffuseStrength)
    TO_JSON(o, specularStrength)
}

void Light::from_json(const json& j, Light& o) {
    FROM_JSON(o, position)
    FROM_JSON(o, color)
    FROM_JSON(o, colorIntensity)
    FROM_JSON(o, glowIntensity)
    FROM_JSON(o, ambientStrength)
    FROM_JSON(o, diffuseStrength)
    FROM_JSON(o, specularStrength)
}

void Light::sync(const Light& otherLight){
    this->position = otherLight.position;
    this->color = otherLight.color;
    this->colorIntensity = otherLight.colorIntensity;
    this->glowIntensity = otherLight.glowIntensity;
    this->ambientStrength = otherLight.ambientStrength;
    this->diffuseStrength = otherLight.diffuseStrength;
    this->specularStrength = otherLight.specularStrength;
}

}  // namespace lenny::gui