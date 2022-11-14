#include <lenny/gui/Utils.h>

#include <glm/gtc/type_ptr.hpp>

namespace lenny::gui::utils {

glm::vec3 toGLM(const Eigen::Vector3d& v) {
    return glm::vec3(v.x(), v.y(), v.z());
}

Eigen::Vector3d toEigen(const glm::vec3& v) {
    return Eigen::Vector3d(v.x, v.y, v.z);
}

glm::mat4 getGLMTransform(const Eigen::Vector3d& position, const Eigen::QuaternionD& orientation, const Eigen::Vector3d& scale) {
    glm::mat4 transform = glm::mat4(1.0);

    //Translate
    transform = glm::translate(transform, toGLM(position));

    //Rotate
    Eigen::AngleAxisd rotation(orientation);
    transform = transform * glm::rotate(glm::mat4(1.0), (float)(rotation.angle()), toGLM(rotation.axis()));

    //Scale
    transform = transform * glm::scale(glm::mat4(1.0), toGLM(scale));

    return transform;
}

}  // namespace lenny::gui::utils