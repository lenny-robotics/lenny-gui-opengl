#pragma once

#include <ImGui.h>
#include <ImGuizmo.h>
#include <lenny/gui/Scene.h>

namespace lenny::gui {

class Guizmo {
private:
    Guizmo() = default;
    ~Guizmo() = default;

public:
    static void useWidget(Eigen::Vector3d& position, Eigen::QuaternionD& orientation, Eigen::Vector3d& scale);

public:
    inline static lenny::gui::Scene* scene = nullptr;  //Set automatically in scene
    inline static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
    inline static ImGuizmo::MODE currentMode = ImGuizmo::LOCAL;
};

}  // namespace lenny::gui