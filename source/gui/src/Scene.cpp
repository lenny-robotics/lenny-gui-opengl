#include <glad/glad.h>
#include <lenny/gui/ImGui.h>
#include <lenny/gui/Renderer.h>
#include <lenny/gui/Scene.h>
#include <lenny/gui/Shaders.h>
#include <lenny/tools/Logger.h>

namespace lenny::gui {

Scene::Scene(const std::string& description, const int& width, const int& height) : description(description), width(width), height(height) {
    //Framebuffer
    glGenFramebuffers(1, &frameBuffer);

    //Texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0); //ToDo: Alpha value?
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

    //Set camera aspect ratio
    camera.setAspectRatio((double)width / (double)height);

    //Update index
    index++;
}

Scene::~Scene() {
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &renderBuffer);
}

void Scene::prepareToDraw(const std::function<void()>& f_drawScene) {
    //Update window infos
    ImGui::Begin(description.c_str());
    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetWindowSize();
    isSelected = ImGui::IsWindowHovered() && (fabs(pos.x - this->windowPos[0]) < 1e-5) && (fabs(pos.y - this->windowPos[1]) < 1e-5);
    this->windowPos = {pos.x, pos.y};
    ImGui::End();

    //Update camera parameters
    camera.setAspectRatio(size.x / (width - size.y));

    //Update shader
    Shaders::update(camera, light);

    //Prepare frame buffer
    LENNY_LOG_DEBUG("Before: %d", frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    LENNY_LOG_DEBUG("After: %d", frameBuffer);
    glViewport(pos.x, -pos.y, size.x, width - size.y);
    glEnable(GL_DEPTH_TEST);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    //Setup default drawings
    if (showGround)
        ground.drawScene();
    if (showOrigin)
        Renderer::I->drawCoordinateSystem(Eigen::Vector3d::Zero(), Eigen::QuaternionD::Identity(), 0.1, 0.01);

    //Draw scene
    if (f_drawScene)
        f_drawScene();

    //Unbind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
}

void Scene::draw() const {
    ImGui::Begin(description.c_str());
    ImGui::GetWindowDrawList()->AddImage((void*)texture, ImVec2(0, 0), ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

void Scene::drawGui() {
    if (ImGui::TreeNode(description.c_str())) {
        ImGui::Checkbox("Show Ground", &showGround);
        ImGui::Checkbox("Show Origin", &showOrigin);

        camera.drawGui();
        light.drawGui();
        ground.drawGui();

        if (ImGui::Button("Print")) {
            camera.printSettings();
            light.printSettings();
            ground.printSettings();
        }

        ImGui::TreePop();
    }
}

void Scene::keyboardKeyCallback(int key, int action) {
    camera.updateKeyboardParameters(key, action);
}

void Scene::mouseButtonCallback(double xPos, double yPos, int button, int action) {
    camera.updateMouseButtonParameters(xPos, yPos, button, action);
}

void Scene::mouseMoveCallback(double xPos, double yPos) {
    if (isSelected)
        camera.processMouseMove(xPos, yPos);
}

void Scene::mouseScrollCallback(double xOffset, double yOffset) {
    if (isSelected)
        camera.processMouseScroll(xOffset, yOffset);
}

}  // namespace lenny::gui