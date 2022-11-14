#pragma once

#include <ImGuizmo.h>
#include <lenny/gui/Utils.h>

namespace ImGuizmo {

//Needs to be executed in the `drawGui` function
void useWidget(Eigen::Vector3d& position, Eigen::QuaternionD& orientation, Eigen::Vector3d& scale, const glm::mat4& cameraView,
               const glm::mat4& cameraProjection);

}  // namespace ImGuizmo