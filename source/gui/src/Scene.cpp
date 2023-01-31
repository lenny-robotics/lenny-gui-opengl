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

#include <glm/gtc/type_ptr.hpp>

namespace lenny::gui {

Scene::Scene(const std::string& description, const int& width, const int& height) : description(description), width(width), height(height) {
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

void Scene::draw(const std::function<void()>& f_drawScene) {
    //Begin ImGui window
    static bool open = true;
    ImGui::Begin(description.c_str(), &open, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

    //Gather window info
    const ImVec2 pos = ImGui::GetWindowPos();
    const ImVec2 size = ImGui::GetContentRegionAvail();

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

    //Guizmo -> AFTER IMAGE!
    {
        //Setup menu
        ImGuizmo::BeginFrame();
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        //Setup operation
        static ImGuizmo::OPERATION currentOperation(ImGuizmo::TRANSLATE);
        if (ImGui::RadioButton("Translate", currentOperation == ImGuizmo::TRANSLATE))
            currentOperation = ImGuizmo::TRANSLATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", currentOperation == ImGuizmo::ROTATE))
            currentOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", currentOperation == ImGuizmo::SCALE))
            currentOperation = ImGuizmo::SCALE;

        //Setup mode
        static ImGuizmo::MODE currentMode(ImGuizmo::LOCAL);
        if (currentOperation == ImGuizmo::SCALE)
            currentMode = ImGuizmo::LOCAL;
        else
            currentMode = ImGuizmo::WORLD;

        //Get glm transformation
        glm::mat4 transform = lenny::gui::utils::getGLMTransform(guizmoPosition, guizmoOrientation, guizmoScale);

        //Draw values in gui
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Translation", matrixTranslation);
        ImGui::InputFloat3("Rotation", matrixRotation);
        ImGui::InputFloat3("Scale", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(transform));

        //Manipulate widget
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        glm::vec2 m_ViewportBounds[2];
        m_ViewportBounds[0] = {viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
        m_ViewportBounds[1] = {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

        ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x,
                          m_ViewportBounds[1].y - m_ViewportBounds[0].y);

        ImGuizmo::Manipulate(glm::value_ptr(camera.getViewMatrix()), glm::value_ptr(camera.getProjectionMatrix()), currentOperation, currentMode,
                             glm::value_ptr(transform), nullptr, nullptr, nullptr, nullptr);

        //Recreate position, orientation and scale
        ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);
        guizmoPosition << matrixTranslation[0], matrixTranslation[1], matrixTranslation[2];
        guizmoOrientation = Eigen::QuaternionD(lenny::tools::utils::rotZ(TO_RAD(matrixRotation[2])) * lenny::tools::utils::rotY(TO_RAD(matrixRotation[1])) *
                                               lenny::tools::utils::rotX(TO_RAD(matrixRotation[0])));
        guizmoScale << matrixScale[0], matrixScale[1], matrixScale[2];
    }

    //Update parameters
    const bool windowIsBeingMoved = (fabs(pos.x - this->windowPos[0]) > 1e-5) || (fabs(pos.y - this->windowPos[1]) > 1e-5);
    blockCameraUpdate = !ImGui::IsWindowHovered() || windowIsBeingMoved || ImGuizmo::IsUsing();
    this->windowPos = {pos.x, pos.y};

    //Wrap up ImGui
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
    if (!blockCameraUpdate)
        camera.updateKeyboardParameters(key, action);
}

void Scene::mouseButtonCallback(double xPos, double yPos, int button, int action) {
    if (!blockCameraUpdate || (action == GLFW_RELEASE))
        camera.updateMouseButtonParameters(xPos, yPos, button, action);
}

void Scene::mouseMoveCallback(double xPos, double yPos) {
    if (!blockCameraUpdate)
        camera.processMouseMove(xPos, yPos);
}

void Scene::mouseScrollCallback(double xOffset, double yOffset) {
    if (!blockCameraUpdate)
        camera.processMouseScroll(xOffset, yOffset);
}

}  // namespace lenny::gui