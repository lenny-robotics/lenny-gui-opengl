#pragma once

#include <GLFW/glfw3.h>
#include <lenny/gui/Process.h>
#include <lenny/gui/Scene.h>

namespace lenny::gui {

class Application {
public:
    Application(const std::string& title, const std::string& iconPath = LENNY_GUI_OPENGL_FOLDER "/data/icons/default_icon.jpg");
    virtual ~Application();

    //--- Run
    void run();

protected:
    //--- Drawing
    virtual void prepareToDraw() {}
    virtual void wrapUpDraw() {}
    virtual void drawGui() {}
    virtual void drawGuizmo() {}
    virtual void drawMenuBar();
    virtual void drawConsole();

    //--- Helpers
    double getDt() const;
    std::pair<int, int> getCurrentWindowPosition() const;
    std::pair<int, int> getCurrentWindowSize() const;
    bool saveScreenshotToFile(const std::string& filePath) const;

private:
    //--- Initialization
    void initializeGLFW(const std::string& title, const std::string& iconPath);
    void initializeOpenGL();
    void initializeImGui();
    void setCallbacks();
    void setGuiAndRenderer();

    //--- Drawing
    void draw();

protected:
    //--- Processes
    std::vector<Process::SPtr> processes;

    //--- Scenes
    std::vector<Scene::SPtr> scenes;

    //--- Framerate
    bool limitFramerate = true;
    double targetFramerate = 60.0;

    //--- Settings
    bool showGui = true;
    bool showConsole = true;
    bool syncScenes = false;

private:
    //--- Window (set in constructor)
    GLFWwindow* glfwWindow = nullptr;

    //--- Framerate
    double currentFramerate = targetFramerate;  //Framerate of drawing process (not for separate thread)
};

}  // namespace lenny::gui