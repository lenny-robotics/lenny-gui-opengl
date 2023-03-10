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

    //--- Drawing
    void draw();
    void drawGui();

    //--- Callbacks
    void resizeWindowCallback(int width, int height);
    void keyboardKeyCallback(int key, int action);
    void mouseButtonCallback(double xPos, double yPos, Ray ray, int button, int action);
    void mouseMoveCallback(double xPos, double yPos, Ray ray);
    void mouseScrollCallback(double xOffset, double yOffset);
    void fileDropCallback(int count, const char** fileNames);

    //--- Helpers
    const Ray getRayFromScreenCoordinates(double xPos, double yPos) const;
    void copyCallbacksFromOtherScene(const Scene::CSPtr otherScene);
    void sync(const Scene::CSPtr otherScene);
    bool saveScreenshotToFile(const std::string& filePath) const;

public:
    //--- Functions
    std::function<void()> f_drawScene;
    std::function<void(int, int)> f_keyboardKeyCallback;
    std::function<void(double, double, Ray, int, int)> f_mouseButtonCallback;
    std::function<void(double, double, Ray)> f_mouseMoveCallback;
    std::function<void(double, double)> f_mouseScrollCallback;
    std::function<void(int, const char**)> f_fileDropCallback;

    const std::string description;
    Camera camera;

    std::array<double, 4> clearColor = {1.0, 1.0, 1.0, 1.0};
    Light light;
    Ground ground;

    bool showGround = true;
    bool showOrigin = true;

private:
    std::array<float, 2> windowPos = {0.f, 0.f}, windowSize = {100.f, 100.f};
    bool blockCallbacks = false;
    uint frameBuffer, texture, renderBuffer;
    int textureWidth, textureHeight;
};

}  // namespace lenny::gui