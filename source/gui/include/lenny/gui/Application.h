#pragma once

#include <GLFW/glfw3.h>
#include <lenny/gui/Scene.h>

#include <thread>

namespace lenny::gui {

class Application {
public:
    Application(const std::string& title);
    virtual ~Application();

    //--- Run
    void run();

protected:
    //--- Process
    virtual void restart() {}
    virtual void process() {}

    //--- Drawing
    virtual void prepareToDraw() {}
    virtual void drawScene() const {}
    virtual void wrapUpDraw() {}
    virtual void drawGui();

    //--- Interaction
    virtual void resizeWindowCallback(int width, int height);
    virtual void keyboardKeyCallback(int key, int action);
    virtual void mouseButtonCallback(double xPos, double yPos, int button, int action);
    virtual void mouseMoveCallback(double xPos, double yPos);
    virtual void mouseScrollCallback(double xOffset, double yOffset);
    virtual void fileDropCallback(int count, const char** fileNames) {}

    //--- Helpers
    double getDt() const;

private:
    //--- Initialization
    void initializeGLFW(const std::string& title);
    void initializeOpenGL();
    void initializeImGui();
    void setCallbacks();
    void setGuiAndRenderer();

    //--- Process
    void baseProcess();
    void startProcess();
    void stopProcess();

    //--- Drawing
    void draw();
    void drawConsole();

    //--- Helpers
    std::pair<int, int> getCurrentWindowPosition() const;
    std::pair<int, int> getCurrentWindowSize() const;

protected:
    //--- Framerate
    bool limitFramerate = true;
    double targetFramerate = 60.0;

    //--- Process
    bool useSeparateProcessThread = false;

    //--- Members
    std::vector<Scene::UPtr> scenes;

    //--- Settings
    bool showConsole = true;

private:
    //--- Window (set in constructor)
    GLFWwindow* glfwWindow = nullptr;

    //--- Framerate
    double currentFramerate = targetFramerate;  //Framerate of drawing process (not for separate thread)

    //--- Process
    std::thread processThread;
    bool processIsRunning = false;
};

}  // namespace lenny::gui