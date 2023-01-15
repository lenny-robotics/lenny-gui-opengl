#include <glad/glad.h>
#include <lenny/gui/ImGui.h>
#include <lenny/gui/Scene.h>
#include <lenny/tools/Logger.h>

namespace lenny::gui {

Scene::Scene(const std::string& description, const int& width, const int& height) : description(description) {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    // The texture we're going to render to
    glGenTextures(1, &texture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, texture);

    // Give an empty image to OpenGL ( the last "0" means "empty" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // The depth buffer
    glGenRenderbuffers(1, &depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderBuffer);

    // Set "texture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture, 0);

    //ToDo: This is probably something that needs to be shifted into the application...
    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);  // "1" is the size of DrawBuffers

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LENNY_LOG_ERROR("Something went wrong when initializing a frame buffer")
}

Scene::~Scene() {
    glDeleteFramebuffers(1, &frameBuffer);
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &depthRenderBuffer);
}

void Scene::draw() {
    ImGui::Begin(description.c_str());
    ImGui::GetWindowPos();
    ImGui::GetWindowDrawList()->AddImage((void*)texture, ImGui::GetWindowPos(), ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}

}  // namespace lenny::gui