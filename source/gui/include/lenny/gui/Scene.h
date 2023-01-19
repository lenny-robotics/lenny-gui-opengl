#pragma once

#include <lenny/gui/Camera.h>
#include <lenny/gui/Ground.h>
#include <lenny/gui/Light.h>
#include <lenny/tools/Typedefs.h>

#include <array>
#include <functional>

namespace lenny::gui {

class Scene {
public:
    LENNY_GENERAGE_TYPEDEFS(Scene)

    Scene(const std::string& description, const int& width, const int& height);
    ~Scene();

    void draw(const std::function<void()>& f_drawScene);
    void drawGui();

    void keyboardKeyCallback(int key, int action);
    void mouseButtonCallback(double xPos, double yPos, int button, int action);
    void mouseMoveCallback(double xPos, double yPos);
    void mouseScrollCallback(double xOffset, double yOffset);

public:
    const std::string description;
    Camera camera;

    std::array<double, 4> clearColor = {1.0, 1.0, 1.0, 1.0};
    Light light;
    Ground ground;

    bool showGround = true;
    bool showOrigin = true;

private:
    const int width, height;
    std::array<float, 2> windowPos = {0.f, 0.f};
    bool isSelected = false;
    uint frameBuffer, texture, renderBuffer;
};

}  // namespace lenny::gui