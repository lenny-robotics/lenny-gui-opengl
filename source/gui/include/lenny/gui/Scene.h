#pragma once

#include <lenny/gui/Camera.h>
#include <lenny/gui/Ground.h>
#include <lenny/gui/Light.h>

#include <array>
#include <functional>

namespace lenny::gui {

class Scene {
public:
    Scene(const std::string& description, const int& width, const int& height);
    ~Scene();

    void prepareToDraw(const std::function<void()>& f_drawScene);
    void draw() const;
    void drawGui();

    void keyboardKeyCallback(int key, int action);
    void mouseButtonCallback(double xPos, double yPos, int button, int action);
    void mouseMoveCallback(double xPos, double yPos);
    void mouseScrollCallback(double xOffset, double yOffset);

public:
    const std::string description;
    Camera camera;

    inline static std::array<double, 4> clearColor = {1.0, 1.0, 1.0, 1.0};
    inline static Light light;
    inline static Ground ground;

    inline static bool showGround = true;
    inline static bool showOrigin = true;

private:
    const int width, height;
    std::array<float, 2> windowPos = {0.f, 0.f};
    bool isSelected = false;

    uint frameBuffer;
    uint texture;
    uint renderBuffer;

    inline static int index = 0;
};

}  // namespace lenny::gui