#include <lenny/gui/ImGui.h>
#include <lenny/gui/ImGuizmo.h>
#include <lenny/tools/Utils.h>

#include <glm/gtc/type_ptr.hpp>

namespace ImGuizmo {

void useWidget(Eigen::Vector3d& position, Eigen::QuaternionD& orientation, Eigen::Vector3d& scale, const glm::mat4& cameraView,
               const glm::mat4& cameraProjection) {
    //Setup menu
    ImGuizmo::BeginFrame();

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
    glm::mat4 transform = lenny::gui::utils::getGLMTransform(position, orientation, scale);

    //Draw values in gui
    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, glm::value_ptr(transform));

    //Manipulate widget
    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), currentOperation, currentMode, glm::value_ptr(transform), nullptr,
                         nullptr, nullptr, nullptr);

    //Recreate position, orientation and scale
    ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform), matrixTranslation, matrixRotation, matrixScale);
    position << matrixTranslation[0], matrixTranslation[1], matrixTranslation[2];
    orientation = Eigen::QuaternionD(lenny::tools::utils::rotZ(TO_RAD(matrixRotation[2])) * lenny::tools::utils::rotY(TO_RAD(matrixRotation[1])) *
                                     lenny::tools::utils::rotX(TO_RAD(matrixRotation[0])));
    scale << matrixScale[0], matrixScale[1], matrixScale[2];
}

}  // namespace ImGuizmo