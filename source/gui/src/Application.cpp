#include <glad/glad.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <lenny/gui/Application.h>
#include <lenny/gui/Gui.h>
#include <lenny/gui/Plot.h>
#include <lenny/gui/Renderer.h>
#include <lenny/gui/Shaders.h>
#include <lenny/tools/Logger.h>
#include <lenny/tools/Timer.h>
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace lenny::gui {

Application::Application(const std::string &title, const std::string &iconPath) {
    //Initialize everything
    initializeGLFW(title, iconPath);
    initializeOpenGL();
    initializeImGui();
    setCallbacks();
    glfwMaximizeWindow(this->glfwWindow);
    Shaders::initialize();
    setGuiAndRenderer();
}

Application::~Application() {
    //Terminate ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    //Terminate glfw
    glfwDestroyWindow(this->glfwWindow);
    glfwTerminate();
}

void Application::initializeGLFW(const std::string &title, const std::string &iconPath) {
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

    //Create window
    this->glfwWindow = glfwCreateWindow(mode->width, mode->height, title.c_str(), nullptr, nullptr);
    if (!this->glfwWindow)
        LENNY_LOG_ERROR("GLFW: Failed to create window!");
    glfwMakeContextCurrent(this->glfwWindow);

    //Setup icon
    const std::ifstream iconFile(iconPath.c_str());
    if (iconFile.good()) {
        GLFWimage image;
        image.pixels = stbi_load(iconPath.c_str(), &image.width, &image.height, nullptr, 4);
        glfwSetWindowIcon(this->glfwWindow, 1, &image);
        stbi_image_free(image.pixels);
    } else {
        LENNY_LOG_WARNING("Icon file path `%s` could not be found", iconPath.c_str())
    }

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
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    //Set font
    const auto [windowWidth, windowHeight] = getCurrentWindowSize();
    const float fontSize = 11.f * (float)windowWidth / (float)windowHeight;
    io.Fonts->AddFontFromFileTTF(IMGUI_FONT_FOLDER "/OpenSans-Bold.ttf", fontSize);
    io.FontDefault = io.Fonts->AddFontFromFileTTF(IMGUI_FONT_FOLDER "/OpenSans-Regular.ttf", fontSize);

    //Set style
    ImGui::StyleColorsDark();

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
        //Set viewport
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        //Scene callbacks
        for (auto &scene : app->scenes)
            scene->resizeWindowCallback(width, height);
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
            glfwSetWindowShouldClose(window, GL_TRUE);
            return;
        }
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            //Processes
            if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
                for (auto &process : app->processes)
                    process->toggle();
            if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
                for (auto &process : app->processes)
                    process->step();
            if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
                for (auto &process : app->processes)
                    process->restart();

            //Scene callbacks
            for (auto &scene : app->scenes)
                scene->keyboardKeyCallback(key, action);

            //Screenshot
            if (key == GLFW_KEY_S && action == GLFW_PRESS)
                app->saveScreenshotToFile(LENNY_PROJECT_FOLDER "/logs/Screenshot-" + tools::utils::getCurrentDateAndTime() + ".png");
        }
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
            //Scene callbacks
            for (auto &scene : app->scenes)
                scene->mouseButtonCallback(xPos, yPos, scene->getRayFromScreenCoordinates(xPos, yPos), button, action);
        }
    });

    //Mouse move
    glfwSetCursorPosCallback(this->glfwWindow, [](GLFWwindow *window, double xPos, double yPos) {
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        io.AddMousePosEvent((float)xPos, (float)yPos);

        //App
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        //Scene callbacks
        for (auto &scene : app->scenes)
            scene->mouseMoveCallback(xPos, yPos, scene->getRayFromScreenCoordinates(xPos, yPos));
    });

    //Mouse scroll
    glfwSetScrollCallback(this->glfwWindow, [](GLFWwindow *window, double xOffset, double yOffset) {
        //ImGui
        ImGuiIO &io = ImGui::GetIO();
        if (io.WantCaptureMouse)
            ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);

        //App
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        //Scene callbacks
        for (auto &scene : app->scenes)
            scene->mouseScrollCallback(xOffset, yOffset);
    });

    //File drop
    glfwSetDropCallback(this->glfwWindow, [](GLFWwindow *window, int count, const char **filenames) {
        //App
        Application *app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        //Scene callbacks
        for (auto &scene : app->scenes)
            scene->fileDropCallback(count, filenames);
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

        //Processes
        for (Process::SPtr process : processes)
            if (!process->separateThreadIsUsed() && process->isRunning())
                process->step();

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

void Application::drawMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Processes")) {
            for (Process::SPtr process : processes)
                process->drawGui();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Scenes")) {
            for (uint i = 0; i < scenes.size(); i++) {
                if (ImGui::TreeNode(scenes.at(i)->description.c_str())) {
                    scenes.at(i)->drawGui();
                    if (ImGui::Button("Remove"))
                        scenes.erase(scenes.begin() + i);
                    ImGui::TreePop();
                }
            }

            ImGui::Separator();
            if (ImGui::Button("Add Scene")) {
                const auto &[width, height] = getCurrentWindowSize();
                scenes.emplace_back(std::make_shared<Scene>("Scene-" + std::to_string(scenes.size() + 1), width, height));
                if (scenes.size() > 1)
                    scenes.back()->copyCallbacksFromOtherScene(scenes.at(scenes.size() - 2));
            }

            ImGui::Separator();
            ImGui::Checkbox("Sync Scenes", &syncScenes);

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Drawing")) {
            static double drawFramerate = currentFramerate;
            static tools::Timer timer;
            if (timer.time() > 0.333) {
                drawFramerate = currentFramerate;
                timer.restart();
            }
            ImGui::Text("Current FPS: %.2f", drawFramerate);
            ImGui::Checkbox("Limit FPS to", &limitFramerate);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50.f);
            ImGui::InputDouble(" ", &targetFramerate, 0.0, 0.0, "%.1f");

            ImGui::Separator();
            ImGui::Checkbox("Show Console", &showConsole);
            ImGui::Checkbox("Show Gui", &showGui);

            ImGui::Separator();
            if (ImGui::Button("Save Screenshot"))
                saveScreenshotToFile(LENNY_PROJECT_FOLDER "/logs/Screenshot-" + tools::utils::getCurrentDateAndTime() + ".png");

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void Application::drawConsole() {
    ImGui::Begin("Console");
    const auto &msgBuffer = tools::Logger::getMessageBuffer();
    for (const auto &[color, msg] : msgBuffer) {
        const auto rgb = tools::Logger::getColorArray(color);
        ImGui::TextColored(ImVec4(rgb[0], rgb[1], rgb[2], 1.0), "%s", msg.c_str());
        if (msg.length() != 0 && msg.back() != '\n')
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

double Application::getDt() const {
    return 1.0 / currentFramerate;
}

bool Application::saveScreenshotToFile(const std::string &filePath) const {
    //Check extension
    if (!tools::utils::checkFileExtension(filePath, "png")) {
        LENNY_LOG_WARNING("Invalid file extension for file path `%s`. It needs to be `png`", filePath.c_str())
        return false;
    }

    //Get pixels
    int width, height;
    glfwGetFramebufferSize(glfwWindow, &width, &height);
    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;
    std::vector<char> buffer(bufferSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

    //Save to file
    stbi_flip_vertically_on_write(true);
    const bool successful = stbi_write_png(filePath.c_str(), width, height, nrChannels, buffer.data(), stride);
    if (successful)
        LENNY_LOG_INFO("Successfully saved screenshot to file `%s`", filePath.c_str())
    else
        LENNY_LOG_WARNING("Could not save screenshot into file `%s`", filePath.c_str())
    return successful;
}

void Application::draw() {
    //Prepare glfw
    const auto [windowWidth, windowHeight] = getCurrentWindowSize();
    if (windowWidth < 1 || windowHeight < 1)
        return;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.f, 0.f, 0.f, 1.f);
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

    //Draw DockSpace
    ImGuiIO &io = ImGui::GetIO();
    ImGuiStyle &style = ImGui::GetStyle();
    const float minWinSizeX = style.WindowMinSize.x;
    style.WindowMinSize.x = (float)windowWidth / 6.f;
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
    }
    style.WindowMinSize.x = minWinSizeX;

    //Draw menu bar
    drawMenuBar();

    //Sync scenes
    if (syncScenes && scenes.size() > 1)
        for (int i = 1; i < scenes.size(); i++)
            scenes.at(i)->sync(scenes.at(0));

    //Draw scenes
    for (auto &scene : scenes) {
        ImGui::SetNextWindowPos(ImVec2(0.25 * windowWidth, ImGui::GetFrameHeight()), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.75 * windowWidth, 0.75 * windowHeight - ImGui::GetFrameHeight()), ImGuiCond_FirstUseEver);
        scene->draw();
    }

    //Draw console
    if (showConsole) {
        ImGui::SetNextWindowPos(ImVec2(0.f, 0.75 * windowHeight), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(windowWidth, 0.25 * windowHeight), ImGuiCond_FirstUseEver);
        drawConsole();
    }

    //Draw gui
    if (showGui) {
        ImGui::SetNextWindowPos(ImVec2(0.f, ImGui::GetFrameHeight()), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(0.25 * windowWidth, 0.75 * windowHeight - ImGui::GetFrameHeight()), ImGuiCond_FirstUseEver);
        drawGui();
    }

    //Draw guizmo
    drawGuizmo();

    //End for docking
    ImGui::End();

    //Wrap up imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}  // namespace lenny::gui
