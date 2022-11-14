#pragma once

#include <lenny/tools/Definitions.h>

#include <glm/glm.hpp>

namespace lenny::gui::utils {

glm::vec3 toGLM(const Eigen::Vector3d& v);
Eigen::Vector3d toEigen(const glm::vec3& v);
glm::mat4 getGLMTransform(const Eigen::Vector3d& position, const Eigen::QuaternionD& orientation, const Eigen::Vector3d& scale);

}  // namespace lenny::gui::utils