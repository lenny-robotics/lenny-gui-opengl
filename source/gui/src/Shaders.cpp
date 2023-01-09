#include <lenny/gui/Shaders.h>

namespace lenny::gui {

std::vector<Shader> Shaders::shaderList = {};

Shader* Shaders::activeShader = nullptr;

void Shaders::initialize() {
    shaderList.clear();
    shaderList.emplace_back(LENNY_GUI_OPENGL_FOLDER "/data/shaders/shader.vert", LENNY_GUI_OPENGL_FOLDER "/data/shaders/shader.frag");

    setActiveShader(BASIC);
}

void Shaders::update(const Camera& camera, const Light& light) {
    shaderList[BASIC].activate();

    shaderList[BASIC].setMat4("cameraProjection", camera.getProjectionMatrix());
    shaderList[BASIC].setMat4("cameraView", camera.getViewMatrix());
    shaderList[BASIC].setVec3("cameraPosition", camera.getPosition());

    shaderList[BASIC].setVec3("lightPosition", light.getPosition());
    shaderList[BASIC].setVec3("lightColor", light.getColor());
    shaderList[BASIC].setVec3("lightGlow", light.getGlow());
    shaderList[BASIC].setFloat("strength.ambient", light.ambientStrength);
    shaderList[BASIC].setFloat("strength.diffuse", light.diffuseStrength);
    shaderList[BASIC].setFloat("strength.specular", light.specularStrength);
}

void Shaders::setActiveShader(SHADERS shader) {
    activeShader = &shaderList[shader];
}

}  // namespace lenny::gui