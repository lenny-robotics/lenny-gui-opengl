#include <glad/glad.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <lenny/gui/Application.h>
#include <lenny/gui/Gui.h>
#include <lenny/gui/ImGui.h>
#include <lenny/gui/Plot.h>
#include <lenny/gui/Renderer.h>
#include <lenny/gui/Shaders.h>
#include <lenny/tools/Logger.h>
#include <lenny/tools/Timer.h>

namespace lenny::gui {

Application::Application(const std::string &title) {
    initializeGLFW(title);
    initializeOpenGL();
    setCallbacks();
    glfwMaximizeWindow(this->glfwWindow);
    setCameraAspectRatio();
    Shaders::initialize();
    initializeImGui();
    setGuiAndRenderer();
}

Application::~Application() {
    //Terminate process
    if (useSeparateProcessThread && processIsRunning) {
        processIsRunning = false;
        processThread.join();
    }

    //Terminate ImGui
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    //Terminate glfw
    glfwTerminate();
}

void Application::initializeGLFW(const std::string &title) {
    //Initialize
    if (!glfwInit())
        LENNY_LOG_ERROR("GLFW: initialization failed!");

    //Get primary monitor
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor)
        LENNY_LOG_ERROR("GLFW: primary monitor could not be found!");

    //Get video mode
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
        LENNY_LOG_ERROR("GLFW: video mode could not be determined!");

    //Set window dimensions
    const int borderLeft = 2;
    const int borderTop = 70;
    const int borderRight = 2;
    const int borderBottom = 105;

    this->width = mode->width - borderLeft - borderRight;
    this->height = mode->height - borderTop - borderBottom;

    //Set glfw window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    //Set pixel ratio
    float xScale, yScale;
    glfwGetMonitorContentScale(monitor, &xScale, &yScale);
    this->pixelRatio = xScale;

    //Create window
    this->glfwWindow = glfwCreateWindow(this->width, this->height, title.c_str(), nullptr, nullptr);
    if (!this->glfwWindow)
        LENNY_LOG_ERROR("GLFW: Failed to create window!");
    glfwMakeContextCurrent(this->glfwWindow);

    //Disable waiting for framerate of glfw window
    glfwSwapInterval(0);

    //Initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        LENNY_LOG_ERROR("Failed to initialize glad!");
}

inline void GLAPIENTRY GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    if (type == GL_DEBUG_TYPE_ERROR)
        LENNY_LOG_WARNING("GL CALLBACK ERROR: %s type = 0x%x, severity = 0x%x, message = %s", type, severity, message);
}

void Application::initializeOpenGL() {
    // Enable error callback
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(GLCallback, 0);

    //Enable gl settings
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
}

void Application::initializeImGui() {
    //Check version
    IMGUI_CHECKVERSION();

    //Create context
    ImGui::CreateContext();
    ImPlot::CreateContext();

    //Set font
    ImGuiIO &io = ImGui::GetIO();
    ImFontConfig cfg;
    cfg.SizePixels = 40.f * this->pixelRatio;
    cfg.GlyphOffset.y = this->pixelRatio;  //Necessary?
    io.Fonts->AddFontFromFileTTF(IMGUI_FONT_FOLDER "/Roboto-Medium.ttf", 15.f * this->pixelRatio, &cfg);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    //Set style
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(this->pixelRatio);

    //Initialize
    ImGui_ImplGlfw_InitForOpenGL(this->glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");
}

void Application::setCallbacks() {
    //User pointer
    glfwSetWindowUserPointer(this->glfwWindow, this);

    //Error
    glfwSetErrorCallback([](int error, const char *description) { LENNY_LOG_WARNING("GLFW CALLBACK: Error %d: %s", error, description); });

    //Resize window
    glfwSetFramebufferSizeCallback(this->glfwWindow, [](GLFWwindow *window, int width, int height) {
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->resizeWindowCallback(width, height);
    });

    //Keyboard key
    glfwSetKeyCallback(this->glfwWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        //Special treatment for escape key (close application)
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_ESCAPE) {
            if (app->useSeparateProcessThread && app->processIsRunning) {
                app->processIsRunning = false;
                app->processThread.join();
            }
            glfwSetWindowShouldClose(window, GL_TRUE);
            return;
        }

        //Update camera parameters
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            app->camera.updateKeyboardParameters(key, action);

        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard || io.WantTextInput) {
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
            return;
        }

        //Call app callback function
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            app->keyboardKeyCallback(key, action);
    });

    //Mouse button
    glfwSetMouseButtonCallback(this->glfwWindow, [](GLFWwindow *window, int button, int action, int mods) {
        //Get app parameters
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        //Update camera parameters
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            app->camera.updateMouseButtonParameters(xPos, yPos, button, action);

        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        io.AddMouseButtonEvent(button, (action == GLFW_PRESS));
        if (io.WantCaptureMouse) {
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
            return;
        }

        //Call app callback function
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            app->mouseButtonCallback(xPos, yPos, button, action);
    });

    //Mouse move
    glfwSetCursorPosCallback(this->glfwWindow, [](GLFWwindow *window, double xPos, double yPos) {
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        io.AddMousePosEvent((float)xPos, (float)yPos);
        if (io.WantCaptureMouse) {
            return;
        }

        //Application
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->mouseMoveCallback(xPos, yPos);
    });

    //Mouse scroll
    glfwSetScrollCallback(this->glfwWindow, [](GLFWwindow *window, double xOffset, double yOffset) {
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        io.AddMouseWheelEvent((float)xOffset, (float)yOffset);
        if (io.WantCaptureMouse) {
            ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
            return;
        }

        //Application
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->mouseScrollCallback(xOffset, yOffset);
    });

    //File drop
    glfwSetDropCallback(this->glfwWindow, [](GLFWwindow *window, int count, const char **filenames) {
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->fileDropCallback(count, filenames);
    });
}

void Application::setGuiAndRenderer() {
    //Gui
    tools::Gui::I.release();
    tools::Gui::I = std::make_unique<gui::Gui>();

    //Renderer
    tools::Renderer::I.release();
    tools::Renderer::I = std::make_unique<gui::Renderer>();
}

void Application::setCameraAspectRatio() {
    camera.setAspectRatio((double)this->width / (double)this->height);
}

void Application::run() {
    tools::Timer timer;
    while (!glfwWindowShouldClose(this->glfwWindow)) {
        //Process
        if (!useSeparateProcessThread && processIsRunning)
            process();

        //Draw
        draw();

        //Swap glfw buffers and poll IO events (keyboard, mouse, etc.)
        glfwSwapBuffers(this->glfwWindow);
        glfwPollEvents();

        //Limit frame rate
        if (limitFramerate && (1.0 / targetFramerate) > timer.time())
            tools::Timer::sleep((1.0 / targetFramerate) - timer.time(), true);

        //Update current frame rate
        currentFramerate = 1.0 / timer.time();
        timer.restart();
    }

    //Terminate glfw
    glfwTerminate();
}

void Application::drawGui() {
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::Begin("Main Menu");

    ImGui::Text("Play:");
    ImGui::SameLine();

    if (ImGui::ToggleButton("Play", &processIsRunning))
        processCallback();

    if (!processIsRunning) {
        ImGui::SameLine();
        if (ImGui::ArrowButton("tmp", ImGuiDir_Right))
            process();
    }

    ImGui::SameLine();
    if (ImGui::Button("Restart"))
        restart();

    if (ImGui::TreeNode("Settings")) {
        camera.drawGui();
        light.drawGui();
        ground.drawGui();

        if (ImGui::Button("Print Settings")) {
            camera.printSettings();
            light.printSettings();
            ground.printSettings();
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Drawing")) {
        ImGui::Checkbox("Show Ground", &showGround);
        ImGui::Checkbox("Show Origin", &showOrigin);
        ImGui::Checkbox("Show FPS", &showFPS);
        ImGui::Checkbox("Show Console", &showConsole);

        ImGui::TreePop();
    }

    ImGui::End();
}

void Application::drawFPS() {
    static double drawFramerate = currentFramerate;
    static tools::Timer timer;
    if (timer.time() > 0.333) {
        drawFramerate = currentFramerate;
        timer.restart();
    }

    ImGui::SetNextWindowPos(ImVec2(this->width - pixelRatio * 200, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(pixelRatio * 200, pixelRatio * 80), ImGuiCond_Always);
    ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once);
    char title[100];
    sprintf(title, "FPS: %.2f###FPS", drawFramerate);
    ImGui::Begin(title);
    ImGui::Checkbox("Limit FPS", &limitFramerate);
    ImGui::SameLine(pixelRatio * 100);
    if (limitFramerate)
        ImGui::InputDouble("Target FPS", &targetFramerate);
    ImGui::End();
}

void Application::drawConsole() {
    ImGui::SetNextWindowSize(ImVec2(this->width / 2, pixelRatio * 350), ImGuiCond_Once);
    ImGui::SetNextWindowPos(ImVec2(this->width / 2, this->height - pixelRatio * 350), ImGuiCond_Once);

    ImGui::Begin("Console");
    const std::vector<std::pair<tools::Logger::COLOR, std::string>> &msgBuffer = tools::Logger::getMessageBuffer();
    for (const auto &[color, msg] : msgBuffer) {
        const Eigen::Vector3d rgb = tools::Logger::getColorVector(color);
        ImGui::TextColored(ImVec4(rgb.x(), rgb.y(), rgb.z(), 1.0), "%s", msg.c_str());
        if (msg.back() != '\n')
            ImGui::SameLine(0.f, 0.f);
    }
    ImGui::End();
}

void Application::resizeWindowCallback(int width, int height) {
    this->width = width;
    this->height = height;
    glViewport(0, 0, width, height);
    camera.setAspectRatio((double)width / (double)height);
}

void Application::keyboardKeyCallback(int key, int action) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        processIsRunning = !processIsRunning;
        processCallback();
    } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && !processIsRunning) {
        process();
    }
}

void Application::mouseMoveCallback(double xPos, double yPos) {
    camera.processMouseMove(xPos, yPos);
}

void Application::mouseScrollCallback(double xOffset, double yOffset) {
    camera.processMouseScroll(xOffset, yOffset);
}

double Application::getDt() const {
    return 1.0 / currentFramerate;
}

void Application::baseProcess() {
    while (processIsRunning)
        process();
}

void Application::processCallback() {
    if (!useSeparateProcessThread)
        return;

    if (processIsRunning) {
        processThread = std::thread(&Application::baseProcess, this);
        LENNY_LOG_INFO("Process thread started...\n");
    } else {
        processThread.join();
        LENNY_LOG_INFO("Process thread terminated...\n");
    }
}

void Application::draw() {
    //--- OpenGL
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    Shaders::update(camera, light);
    prepareToDraw();

    if (showGround)
        ground.drawScene();
    if (showOrigin)
        Renderer::I->drawCoordinateSystem(Eigen::Vector3d::Zero(), Eigen::QuaternionD::Identity(), 0.1, 0.01);
    drawScene();

    //--- ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (showFPS)
        drawFPS();
    if (showConsole)
        drawConsole();
    drawGui();

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace lenny::gui
