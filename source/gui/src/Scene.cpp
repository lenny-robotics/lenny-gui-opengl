#ifdef WIN32
#pragma warning(disable : 4312)
#endif

// clang-format off
#include <glad/glad.h> //Glad needs to be included before glfw!
#include <GLFW/glfw3.h>
// clang-format on

#include <lenny/gui/ImGui.h>
#include <lenny/gui/ImGuizmo.h>
#include <lenny/gui/Renderer.h>
#include <lenny/gui/Scene.h>
#include <lenny/gui/Shaders.h>
#include <lenny/tools/Logger.h>

namespace lenny::gui {

Scene::Scene(const std::string& description, const int& width, const int& height) : description(description) {
    //Framebuffer
    glGenFramebuffers(1, &frameBuffer);

    //Texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Renderbuffer
    glGenRenderbuffers(1, &renderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    //Attach texture and renderbuffer to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

    //Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LENNY_LOG_ERROR("Something went wrong when initializing a frame buffer")
}

Scene::~Scene() {
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &renderBuffer);
}

void Scene::draw() {
    //Begin ImGui window
    ImGui::Begin(description.c_str(), &open, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);
    //ToDo: Set initial size and position (and maybe docking?)

    //Gather window info
    const bool isTitleBarHovered = ImGui::IsItemHovered();
    const ImVec2 vPos = ImGui::GetWindowPos();
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const ImVec2 pos(vPos.x + vMin.x, vPos.y + vMin.y);
    const ImVec2 size(vMax.x - vMin.x, vMax.y - vMin.y);

    //Update camera parameters
    camera.setAspectRatio(size.x / size.y);

    //Update shader
    Shaders::update(camera, light);

    //Prepare frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //Setup default drawings
    if (showGround)
        ground.drawScene();
    if (showOrigin)
        Renderer::I->drawCoordinateSystem(Eigen::Vector3d::Zero(), Eigen::QuaternionD::Identity(), 0.1, 0.01);

    //Render scene
    if (f_drawScene)
        f_drawScene();

    //Unbind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Draw texture
    ImGui::Image((ImTextureID)texture, size, ImVec2(0, 1), ImVec2(1, 0));

    //Update parameters
    blockCallbacks = !ImGui::IsWindowHovered() || isTitleBarHovered || ImGuizmo::IsUsing() || ImGuizmo::IsOver();
    this->windowPos = {pos.x, pos.y};
    this->windowSize = {size.x, size.y};

    //Wrap up ImGui
    ImGui::End();
}

void Scene::drawGui() {
    ImGui::Checkbox("Show Ground", &showGround);
    ImGui::Checkbox("Show Origin", &showOrigin);

    camera.drawGui();
    light.drawGui();
    ground.drawGui();

    if (ImGui::Button("Print")) { //ToDo: Shift this into individual classes
        camera.printSettings();
        light.printSettings();
        ground.printSettings();
    }
}

void Scene::resizeWindowCallback(int width, int height) {
    //Update texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    //Update renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void Scene::keyboardKeyCallback(int key, int action) {
    if (!blockCallbacks && f_keyboardKeyCallback)
        f_keyboardKeyCallback(key, action);
    if (!blockCallbacks)
        camera.updateKeyboardParameters(key, action);
}

void Scene::mouseButtonCallback(double xPos, double yPos, int button, int action) {
    if (!blockCallbacks && f_mouseButtonCallback)
        f_mouseButtonCallback(xPos, yPos, button, action);
    if (!blockCallbacks || (action == GLFW_RELEASE))
        camera.updateMouseButtonParameters(xPos, yPos, button, action);
}

void Scene::mouseMoveCallback(double xPos, double yPos) {
    if (!blockCallbacks && f_mouseMoveCallback)
        f_mouseMoveCallback(xPos, yPos);
    if (!blockCallbacks)
        camera.processMouseMove(xPos, yPos);
}

void Scene::mouseScrollCallback(double xOffset, double yOffset) {
    if (!blockCallbacks && f_mouseScrollCallback)
        f_mouseScrollCallback(xOffset, yOffset);
    if (!blockCallbacks)
        camera.processMouseScroll(xOffset, yOffset);
}

void Scene::fileDropCallback(int count, const char** fileNames) {
    if (!blockCallbacks && f_fileDropCallback)
        f_fileDropCallback(count, fileNames);
}

const Ray Scene::getRayFromScreenCoordinates(double xPos, double yPos) const{
    glm::vec4 viewportParams(0, 0, windowSize[0], windowSize[1]);
    return camera.getRayFromScreenCoordinates(xPos - windowPos[0], yPos - windowPos[1], viewportParams);
}

}  // namespace lenny::gui