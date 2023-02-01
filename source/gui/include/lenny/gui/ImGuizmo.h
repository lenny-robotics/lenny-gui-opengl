#pragma once

#include <ImGuizmo.h>
#include <lenny/gui/Scene.h>

namespace ImGuizmo {

//Needs to be executed in the `drawGui` function
void useWidget(Eigen::Vector3d& position, Eigen::QuaternionD& orientation, Eigen::Vector3d& scale, const lenny::gui::Scene::CSPtr scene);

}  // namespace ImGuizmo