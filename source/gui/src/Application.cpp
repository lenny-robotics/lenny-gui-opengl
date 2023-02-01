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
    initializeImGui();
    setCallbacks();
    glfwMaximizeWindow(this->glfwWindow);
    Shaders::initialize();
    setGuiAndRenderer();

    const auto [width, height] = getCurrentWindowSize();
    scenes.emplace_back(std::make_shared<Scene>("Scene-1", width, height));
    scenes.emplace_back(std::make_shared<Scene>("Scene-2", width, height));
}

Application::~Application() {
    //Stop process
    stopProcess();

    //Terminate ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    //Terminate glfw
    glfwDestroyWindow(this->glfwWindow);
    glfwTerminate();
}

void Application::initializeGLFW(const std::string &title) {
    //Initialize
    if (!glfwInit())
        LENNY_LOG_ERROR("GLFW: initialization failed!");

    //Set glfw window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4.6);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4.6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

    //Get primary monitor
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    if (!monitor)
        LENNY_LOG_ERROR("GLFW: primary monitor could not be found!");

    //Get video mode
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    if (!mode)
        LENNY_LOG_ERROR("GLFW: video mode could not be determined!");

    //Get window dimensions //ToDo: Is this still necessary?
    const int borderLeft = 2;
    const int borderTop = 70;
    const int borderRight = 2;
    const int borderBottom = 105;

    const int width = mode->width - borderLeft - borderRight;
    const int height = mode->height - borderTop - borderBottom;

    //Create window
    this->glfwWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!this->glfwWindow)
        LENNY_LOG_ERROR("GLFW: Failed to create window!");
    glfwMakeContextCurrent(this->glfwWindow);

    //Disable waiting for framerate of glfw window
    glfwSwapInterval(0);
}

inline void GLAPIENTRY GLCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    if (type == GL_DEBUG_TYPE_ERROR)
        LENNY_LOG_WARNING("GL CALLBACK ERROR: type = '0x%x', severity = '0x%x', message = '%s'", type, severity, message);
}

void Application::initializeOpenGL() {
    //Initialize glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        LENNY_LOG_ERROR("Failed to initialize glad!");

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

    //Set io
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    //Set font
    float fontSize = 18.0f;
    io.Fonts->AddFontFromFileTTF(IMGUI_FONT_FOLDER "/OpenSans-Bold.ttf", fontSize);
    io.FontDefault = io.Fonts->AddFontFromFileTTF(IMGUI_FONT_FOLDER "/OpenSans-Regular.ttf", fontSize);

    //Set style
    ImGui::StyleColorsDark();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    //Set colors
    auto &colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    //Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    //Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    //Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

    //Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};

    //Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.15f, 0.1505f, 0.151f, 1.0f};

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
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureKeyboard || io.WantTextInput)
            ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

        //App
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_ESCAPE) {
            app->stopProcess();
            glfwSetWindowShouldClose(window, GL_TRUE);
            return;
        }
        if (action == GLFW_PRESS || action == GLFW_RELEASE)
            app->keyboardKeyCallback(key, action);
    });

    //Mouse button
    glfwSetMouseButtonCallback(this->glfwWindow, [](GLFWwindow *window, int button, int action, int mods) {
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);

        //App
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            double xPos, yPos;
            glfwGetCursorPos(window, &xPos, &yPos);
            app->mouseButtonCallback(xPos, yPos, button, action);
        }
    });

    //Mouse move
    glfwSetCursorPosCallback(this->glfwWindow, [](GLFWwindow *window, double xPos, double yPos) {
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        io.AddMousePosEvent((float)xPos, (float)yPos);

        //App
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->mouseMoveCallback(xPos, yPos);
    });

    //Mouse scroll
    glfwSetScrollCallback(this->glfwWindow, [](GLFWwindow *window, double xOffset, double yOffset) {
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);

        //App
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        app->mouseScrollCallback(xOffset, yOffset);
    });

    //File drop
    glfwSetDropCallback(this->glfwWindow, [](GLFWwindow *window, int count, const char **filenames) {
        //App
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

void Application::run() {
    tools::Timer timer;
    while (!glfwWindowShouldClose(this->glfwWindow)) {
        //Poll IO events (keyboard, mouse, etc.)
        glfwPollEvents();

        //Process
        if (!useSeparateProcessThread && processIsRunning)
            process();

        //Draw
        prepareToDraw();
        draw();
        wrapUpDraw();

        //Swap glfw buffers
        glfwSwapBuffers(this->glfwWindow);

        //Limit frame rate
        if (limitFramerate && (1.0 / targetFramerate) > timer.time())
            tools::Timer::sleep((1.0 / targetFramerate) - timer.time(), true);

        //Update current frame rate
        currentFramerate = 1.0 / timer.time();
        timer.restart();
    }
}

void Application::drawGui() {
    ImGui::Begin("Main Menu");

    static double drawFramerate = currentFramerate;
    static tools::Timer timer;
    if (timer.time() > 0.333) {
        drawFramerate = currentFramerate;
        timer.restart();
    }
    ImGui::Text("FPS: %.2f", drawFramerate);
    ImGui::Checkbox("Limit FPS", &limitFramerate);
    if (limitFramerate) {
        ImGui::SameLine();
        ImGui::InputDouble("Target FPS", &targetFramerate);
    }

    ImGui::Text("Play:");
    ImGui::SameLine();

    if (ImGui::ToggleButton("Play", &processIsRunning))
        processIsRunning ? startProcess() : stopProcess();

    if (!processIsRunning) {
        ImGui::SameLine();
        if (ImGui::ArrowButton("tmp", ImGuiDir_Right))
            process();
    }

    ImGui::SameLine();
    if (ImGui::Button("Restart"))
        restart();

    if (ImGui::TreeNode("Settings")) {
        ImGui::Checkbox("Show Console", &showConsole);

        ImGui::TreePop();
    }

    for (auto &scene : scenes)
        scene->drawGui();

    ImGui::End();
}

void Application::drawConsole() {
    ImGui::Begin("Console");
    const auto &msgBuffer = tools::Logger::getMessageBuffer();
    for (const auto &[color, msg] : msgBuffer) {
        const auto rgb = tools::Logger::getColorArray(color);
        ImGui::TextColored(ImVec4(rgb[0], rgb[1], rgb[2], 1.0), "%s", msg.c_str());
        if (msg.back() != '\n')
            ImGui::SameLine(0.f, 0.f);
    }
    ImGui::End();
}

std::pair<int, int> Application::getCurrentWindowPosition() const {
    int pos_x, pos_y;
    glfwGetWindowPos(this->glfwWindow, &pos_x, &pos_y);
    return {pos_x, pos_y};
}

std::pair<int, int> Application::getCurrentWindowSize() const {
    int width, height;
    glfwGetFramebufferSize(this->glfwWindow, &width, &height);
    return {width, height};
}

void Application::resizeWindowCallback(int width, int height) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
}

void Application::keyboardKeyCallback(int key, int action) {
    for (auto &scene : scenes)
        scene->keyboardKeyCallback(key, action);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        processIsRunning ? stopProcess() : startProcess();
    } else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS && !processIsRunning) {
        process();
    }
}

void Application::mouseButtonCallback(double xPos, double yPos, int button, int action) {
    for (auto &scene : scenes)
        scene->mouseButtonCallback(xPos, yPos, button, action);
}

void Application::mouseMoveCallback(double xPos, double yPos) {
    for (auto &scene : scenes)
        scene->mouseMoveCallback(xPos, yPos);
}

void Application::mouseScrollCallback(double xOffset, double yOffset) {
    for (auto &scene : scenes)
        scene->mouseScrollCallback(xOffset, yOffset);
}

double Application::getDt() const {
    return 1.0 / currentFramerate;
}

void Application::baseProcess() {
    while (processIsRunning)
        process();
}

void Application::startProcess() {
    if (processIsRunning)
        return;

    processIsRunning = true;
    if (useSeparateProcessThread) {
        processThread = std::thread(&Application::baseProcess, this);
        LENNY_LOG_INFO("Process thread started...\n");
    }
}

void Application::stopProcess() {
    if (!processIsRunning)
        return;

    processIsRunning = false;
    if (useSeparateProcessThread) {
        processThread.join();
        LENNY_LOG_INFO("Process thread terminated...\n");
    }
}

void Application::draw() {
    //Prepare glfw
    const auto [windowWidth, windowHeight] = getCurrentWindowSize();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Prepare imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //Setup dock space window
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags dockSpaceWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    dockSpaceWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    dockSpaceWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    static bool dockspaceOpen = true;
    ImGui::Begin("DockSpace", &dockspaceOpen, dockSpaceWindowFlags);
    ImGui::PopStyleVar(3);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();
    float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = 370.0f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
    }
    style.WindowMinSize.x = minWinSizeX;

    //Menu bar
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Project...", "Ctrl+O")) {
            }
            ImGui::Separator();

            if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
            }

            if (ImGui::MenuItem("Save Scene", "Ctrl+S")) {
            }

            if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S")) {
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Exit")) {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Script")) {
            if (ImGui::MenuItem("Reload assembly", "Ctrl+R")) {
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    //Draw scenes
    for (auto &scene : scenes)
        scene->draw([&]() { drawScene(); });

    //Draw console
    if (showConsole)
        drawConsole();

    //Draw gui
    drawGui();

    //End for docking
    ImGui::End();

    //Wrap up imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *currentContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(currentContext);
    }
}

}  // namespace lenny::gui
