#include <lenny/gui/ImGui.h>
#include <lenny/gui/Guizmo.h>
#include <lenny/tools/Utils.h>

#include <glm/gtc/type_ptr.hpp>

namespace lenny::gui {

void Guizmo::useWidget(Eigen::Vector3d& position, Eigen::QuaternionD& orientation, Eigen::Vector3d& scale) {
    //--- Check if scene has been set
    if(!scene)
        return;

    //--- Transform components
    glm::mat4 transform = lenny::gui::utils::getGLMTransform(position, orientation, scale);

    //--- Draw settings gui
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250.f, 250.f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Guizmo");

    //Setup operation
    if (ImGui::RadioButton("Translate", currentOperation == ImGuizmo::TRANSLATE))
        currentOperation = ImGuizmo::TRANSLATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Rotate", currentOperation == ImGuizmo::ROTATE))
        currentOperation = ImGuizmo::ROTATE;
    ImGui::SameLine();
    if (ImGui::RadioButton("Scale", currentOperation == ImGuizmo::SCALE))
        currentOperation =ImGuizmo::SCALE;

    //Setup mode
    if (currentOperation == ImGuizmo::SCALE)
        currentMode = ImGuizmo::LOCAL;
    else
        currentMode = ImGuizmo::WORLD;

    //Draw values in gui
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Translation", matrixTranslation);
    ImGui::InputFloat3("Rotation", matrixRotation);
    ImGui::InputFloat3("Scale", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(transform));

    //Wrap up
    ImGui::End();

    //--- Draw manipulation gui
    ImGui::Begin(scene->description.c_str());

    //Setup Guizmo
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();

    //Manipulate widget
    const ImVec2 viewportMinRegion = ImGui::GetWindowContentRegionMin();
    const ImVec2 viewportMaxRegion = ImGui::GetWindowContentRegionMax();
    const ImVec2 viewportOffset = ImGui::GetWindowPos();
    const glm::vec2 viewportBounds[2] = {{viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y},
                                         {viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y}};
    ImGuizmo::SetRect(viewportBounds[0].x, viewportBounds[0].y, viewportBounds[1].x - viewportBounds[0].x, viewportBounds[1].y - viewportBounds[0].y);
    ImGuizmo::Manipulate(glm::value_ptr(scene->camera.getViewMatrix()), glm::value_ptr(scene->camera.getProjectionMatrix()), currentOperation, currentMode,
                         glm::value_ptr(transform), nullptr, nullptr, nullptr, nullptr);

    //Wrap up
    ImGui::End();

    //--- Decompose components
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);
    position << matrixTranslation[0], matrixTranslation[1], matrixTranslation[2];
    orientation = Eigen::QuaternionD(lenny::tools::utils::rotZ(TO_RAD(matrixRotation[2])) * lenny::tools::utils::rotY(TO_RAD(matrixRotation[1])) *
                                     lenny::tools::utils::rotX(TO_RAD(matrixRotation[0])));
    scale << matrixScale[0], matrixScale[1], matrixScale[2];
}

}  // namespace lenny::gui