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
    virtual void process() {} //ToDo: Should we enable several process threads? Or remove external thread here and make its own class out of it?
    void startProcess();
    void stopProcess();

    //--- Drawing
    virtual void prepareToDraw() {}
    virtual void wrapUpDraw() {}
    virtual void drawMenuBar();
    virtual void drawGui();
    virtual void drawConsole();

    //--- Helpers
    double getDt() const;
    std::pair<int, int> getCurrentWindowPosition() const;
    std::pair<int, int> getCurrentWindowSize() const;

private:
    //--- Initialization
    void initializeGLFW(const std::string& title);
    void initializeOpenGL();
    void initializeImGui();
    void setCallbacks();
    void setGuiAndRenderer();

    //--- Process
    void baseProcess();

    //--- Drawing
    void draw();

protected:
    //--- Framerate
    bool limitFramerate = true;
    double targetFramerate = 60.0;

    //--- Process
    bool useSeparateProcessThread = false;

    //--- Members
    std::vector<Scene::SPtr> scenes;

    //--- Settings
    bool showMenuBar = true;
    bool showGui = true;
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