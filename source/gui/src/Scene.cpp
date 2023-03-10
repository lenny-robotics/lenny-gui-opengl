#ifdef WIN32
#pragma warning(disable : 4312)
#else
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

// clang-format off
#include <glad/glad.h> //Glad needs to be included before glfw!
#include <GLFW/glfw3.h>
#include <lenny/gui/ImGui.h>
#include <lenny/gui/Guizmo.h>
// clang-format on

#include <lenny/gui/Renderer.h>
#include <lenny/gui/Scene.h>
#include <lenny/gui/Shaders.h>
#include <lenny/tools/Logger.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>

namespace lenny::gui {

Scene::Scene(const std::string& description, const int& width, const int& height) : description(description), textureWidth(width), textureHeight(height) {
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
    ImGui::Begin(description.c_str(), nullptr, ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar);

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
    if (ImGui::IsWindowHovered())
        Guizmo::scene = this;
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

    if(ImGui::Button("Save Screenshot"))
        saveScreenshotToFile(LENNY_PROJECT_FOLDER"/logs/Screenshot-" + description + "-" + tools::utils::getCurrentDateAndTime() + ".png");
}

void Scene::resizeWindowCallback(int width, int height) {
    //Update parameters
    this->textureWidth = width;
    this->textureHeight = height;

    //Update texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    //Update renderbuffer
    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

void Scene::keyboardKeyCallback(int key, int action) {
    if (blockCallbacks)
        return;

    if (f_keyboardKeyCallback)
        f_keyboardKeyCallback(key, action);
    camera.updateKeyboardParameters(key, action);
}

void Scene::mouseButtonCallback(double xPos, double yPos, Ray ray, int button, int action) {
    if (!blockCallbacks && f_mouseButtonCallback)
        f_mouseButtonCallback(xPos, yPos, ray, button, action);
    if (!blockCallbacks || (action == GLFW_RELEASE))
        camera.updateMouseButtonParameters(xPos, yPos, button, action);
}

void Scene::mouseMoveCallback(double xPos, double yPos, Ray ray) {
    if (blockCallbacks)
        return;

    if (f_mouseMoveCallback)
        f_mouseMoveCallback(xPos, yPos, ray);
    camera.processMouseMove(xPos, yPos);
}

void Scene::mouseScrollCallback(double xOffset, double yOffset) {
    if (blockCallbacks)
        return;

    if (f_mouseScrollCallback)
        f_mouseScrollCallback(xOffset, yOffset);
    camera.processMouseScroll(xOffset, yOffset);
}

void Scene::fileDropCallback(int count, const char** fileNames) {
    if (!blockCallbacks && f_fileDropCallback)
        f_fileDropCallback(count, fileNames);
}

const Ray Scene::getRayFromScreenCoordinates(double xPos, double yPos) const {
    glm::vec4 viewportParams(0, 0, windowSize[0], windowSize[1]);
    return camera.getRayFromScreenCoordinates(xPos - windowPos[0], yPos - windowPos[1], viewportParams);
}

void Scene::copyCallbacksFromOtherScene(const Scene::CSPtr otherScene) {
    this->f_drawScene = otherScene->f_drawScene;
    this->f_keyboardKeyCallback = otherScene->f_keyboardKeyCallback;
    this->f_mouseButtonCallback = otherScene->f_mouseButtonCallback;
    this->f_mouseMoveCallback = otherScene->f_mouseMoveCallback;
    this->f_mouseScrollCallback = otherScene->f_mouseScrollCallback;
    this->f_fileDropCallback = otherScene->f_fileDropCallback;
}

void Scene::sync(const Scene::CSPtr otherScene) {
    this->camera.sync(otherScene->camera);
    this->light.sync(otherScene->light);
    this->ground.sync(otherScene->ground);
    this->clearColor = otherScene->clearColor;
    this->showGround = otherScene->showGround;
    this->showOrigin = otherScene->showOrigin;
}

bool Scene::saveScreenshotToFile(const std::string& filePath) const {
    //Bind frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    const GLsizei nrChannels = 3;

    //Get image
    GLsizei stride_input = nrChannels * textureWidth;
    stride_input += (stride_input % 4) ? (4 - stride_input % 4) : 0;
    GLsizei bufferSize_input = stride_input * textureHeight;
    std::vector<unsigned char> buffer_input(bufferSize_input);
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, textureWidth, textureHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer_input.data());

    //Resize image
    GLsizei stride_output = nrChannels * windowSize[0];
    stride_output += (stride_output % 4) ? (4 - stride_output % 4) : 0;
    GLsizei bufferSize_output = stride_output * windowSize[1];
    std::vector<unsigned char> buffer_output(bufferSize_output);
    stbir_resize_uint8(buffer_input.data(), textureWidth, textureHeight, stride_input, buffer_output.data(), windowSize[0], windowSize[1], stride_output, nrChannels);

    //Save image to file
    stbi_flip_vertically_on_write(true);
    const bool successful = stbi_write_png(filePath.c_str(), windowSize[0], windowSize[1], nrChannels, buffer_output.data(), stride_output);
    if (successful)
        LENNY_LOG_INFO("Successfully saved screenshot to file `%s`", filePath.c_str())
    else
        LENNY_LOG_WARNING("Could not save screenshot into file `%s`", filePath.c_str())

    //Deactivate frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return successful;
}

}  // namespace lenny::gui