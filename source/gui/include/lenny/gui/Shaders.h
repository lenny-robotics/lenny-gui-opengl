#pragma once

#include <lenny/gui/Camera.h>
#include <lenny/gui/Light.h>
#include <lenny/gui/Shader.h>

#include <vector>

namespace lenny::gui {

class Shaders {
private:  //Make constructor private, since we want to this to be a purely static class
    Shaders() = default;
    ~Shaders() = default;

private:
    static std::vector<Shader> shaderList;

public:
    enum SHADERS { BASIC };
    static Shader* activeShader;

public:
    static void initialize();
    static void update(const Camera& camera, const Light& light);
    static void setActiveShader(SHADERS shader);
};

}  // namespace lenny::gui