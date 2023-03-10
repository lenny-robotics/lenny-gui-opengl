#include <lenny/gui/Model.h>
#include <lenny/gui/Renderer.h>
#include <lenny/gui/Utils.h>

namespace lenny::gui {

void Renderer::drawCuboid(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const Eigen::Vector3d& dimensions,
                          const Eigen::Vector4d& color) const {
    static Model cube(LENNY_GUI_OPENGL_FOLDER "/data/meshes/cube.obj");
    cube.draw(COM, orientation, dimensions, color.segment(0, 3), color[3]);
}

void Renderer::drawPlane(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const Eigen::Vector2d& dimensions,
                         const Eigen::Vector4d& color) const {
    drawCuboid(COM, orientation, Eigen::Vector3d(dimensions[0], 1e-3, dimensions[1]), color);
}

void Renderer::drawSphere(const Eigen::Vector3d& position, const double& radius, const Eigen::Vector4d& color) const {
    static Model sphere(LENNY_GUI_OPENGL_FOLDER "/data/meshes/sphere.obj");
    sphere.draw(position, Eigen::QuaternionD::Identity(), 2.0 * radius * Eigen::Vector3d::Ones(), color.segment(0, 3), color[3]);
}

void Renderer::drawEllipsoid(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const Eigen::Vector3d& dimensions,
                             const Eigen::Vector4d& color) const {
    static Model sphere(LENNY_GUI_OPENGL_FOLDER "/data/meshes/sphere.obj");
    sphere.draw(COM, orientation, 2.0 * dimensions, color.segment(0, 3), color[3]);
}

void Renderer::drawCylinder(const Eigen::Vector3d& startPosition, const Eigen::Vector3d& endPosition, const double& radius,
                            const Eigen::Vector4d& color) const {
    static Model cylinder(LENNY_GUI_OPENGL_FOLDER "/data/meshes/cylinder.obj");

    Eigen::Vector3d dir = endPosition - startPosition;
    double s = dir.norm();
    if (s < 10e-10)
        return;
    Eigen::Vector3d a = dir.normalized();
    Eigen::Vector3d b = Eigen::Vector3d::UnitZ();
    Eigen::Vector3d v = (b.cross(a)).normalized();
    if (v.norm() < 0.5)
        v = Eigen::Vector3d::UnitX();
    double angle = acos(b.dot(a) / (b.norm() * a.norm()));

    cylinder.draw(startPosition, Eigen::QuaternionD(Eigen::AngleAxisd(angle, v)), Eigen::Vector3d(radius, radius, s), color.segment(0, 3), color[3]);
}

void Renderer::drawCylinder(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const double& height, const double& radius,
                            const Eigen::Vector4d& color) const {
    Eigen::Vector3d startPosition = COM + orientation * Eigen::Vector3d(0.0, height / 2.0, 0.0);
    Eigen::Vector3d endPosition = COM - orientation * Eigen::Vector3d(0.0, height / 2.0, 0.0);
    drawCylinder(startPosition, endPosition, radius, color);
}

void Renderer::drawTetrahedron(const std::array<Eigen::Vector3d, 4>& globalPoints, const Eigen::Vector4d& color) const {
    //Generate model
    static std::vector<Model::Mesh::Vertex> vertices(4, Model::Mesh::Vertex());
    static const std::vector<uint> indices = {0, 1, 2, 1, 2, 3, 0, 1, 3, 0, 2, 3};
    static Model model({{vertices, indices}});

    //Update vertices
    static const glm::vec3 normal = utils::toGLM(Eigen::Vector3d::Ones().normalized());
    for (int i = 0; i < 4; i++) {
        vertices.at(i).position = utils::toGLM(globalPoints.at(i));
        vertices.at(i).normal = normal;
    }
    model.meshes.at(0) = Model::Mesh(vertices, indices);

    //Draw model
    static const Eigen::Vector3d position = Eigen::Vector3d::Zero();
    static const Eigen::QuaternionD orientation = Eigen::QuaternionD::Identity();
    static const Eigen::Vector3d scale = Eigen::Vector3d::Ones();
    model.draw(position, orientation, scale, color.segment(0, 3), color[3]);
}

void Renderer::drawCone(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction, const double& radius, const Eigen::Vector4d& color) const {
    static Model cone(LENNY_GUI_OPENGL_FOLDER "/data/meshes/cone.obj");

    double s = direction.norm();
    if (s < 10e-10)
        return;
    Eigen::Vector3d a = direction.normalized();
    Eigen::Vector3d b = Eigen::Vector3d::UnitY();
    Eigen::Vector3d v = (b.cross(a)).normalized();
    if (v.norm() < 0.5)
        v = Eigen::Vector3d::UnitX();
    double angle = acos(b.dot(a) / (b.norm() * a.norm()));

    cone.draw(origin, Eigen::QuaternionD(Eigen::AngleAxisd(angle, v)), 1e-3 * Eigen::Vector3d(radius, s, radius), color.segment(0, 3), color[3]);
}

void Renderer::drawArrow(const Eigen::Vector3d& startPosition, const Eigen::Vector3d& direction, const double& radius, const Eigen::Vector4d& color) const {
    Eigen::Vector3d dir = direction;
    for (uint i = 0; i < 3; i++)
        if (fabs(dir[i]) < 1e-6)
            dir[i] = 1e-6;

    double coneRadius = 1.5 * radius;
    Eigen::Vector3d coneDir = dir / dir.norm() * coneRadius * 1.5;
    Eigen::Vector3d cyl_endPos = startPosition + dir - coneDir;

    drawCylinder(startPosition, cyl_endPos, radius, color);
    drawCone(cyl_endPos, coneDir, coneRadius, color);
}

void Renderer::drawCoordinateSystem(const Eigen::Vector3d& origin, const Eigen::QuaternionD& orientation, const double& length, const double& radius,
                                    double alpha) const {
    drawArrow(origin, length * orientation.matrix().col(0), radius, Eigen::Vector4d(0.75, 0.0, 0.0, alpha));
    drawArrow(origin, length * orientation.matrix().col(1), radius, Eigen::Vector4d(0.0, 0.75, 0.0, alpha));
    drawArrow(origin, length * orientation.matrix().col(2), radius, Eigen::Vector4d(0.0, 0.0, 0.75, alpha));
}

void Renderer::drawCapsule(const Eigen::Vector3d& startPosition, const Eigen::Vector3d& endPosition, const double& radius, const Eigen::Vector4d& color) const {
    drawCylinder(startPosition, endPosition, radius, color);
    drawSphere(startPosition, radius, color);
    drawSphere(endPosition, radius, color);
}

void Renderer::drawCapsule(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const double& height, const double& radius,
                           const Eigen::Vector4d& color) const {
    Eigen::Vector3d startPosition = COM + orientation * Eigen::Vector3d(0.0, height / 2.0, 0.0);
    Eigen::Vector3d endPosition = COM - orientation * Eigen::Vector3d(0.0, height / 2.0, 0.0);
    drawCapsule(startPosition, endPosition, radius, color);
}

void Renderer::drawLine(const std::vector<Eigen::Vector3d>& linePoints, const double& radius, const Eigen::Vector4d& color) const {
    for (uint i = 0; i < linePoints.size() - 1; i++)
        drawCylinder(linePoints[i], linePoints[i + 1], radius, color);
}

void Renderer::drawTrajectory(const std::vector<Eigen::Vector3d>& trajectoryPoints, const double& radius, const Eigen::Vector4d& color,
                              const bool& showDots) const {
    drawLine(trajectoryPoints, radius, color);
    if (showDots) {
        const double dotRadius = 2.0 * radius;
        for (uint i = 0; i < trajectoryPoints.size(); i++)
            drawSphere(trajectoryPoints[i], dotRadius, color);
    }
}

void Renderer::drawSector(const Eigen::Vector3d& center, const Eigen::QuaternionD& orientation, const double& radius,
                          const std::pair<double, double>& angleRange, const Eigen::Vector4d& color) const {
    static Model sector(LENNY_GUI_OPENGL_FOLDER "/data/meshes/sector.obj");
    for (double angle = angleRange.first; angle < angleRange.second; angle += PI / 180.0) {
        sector.draw(center, orientation * tools::utils::getRotationQuaternion(angle, Eigen::Vector3d::UnitY()), 1e-3 * radius * Eigen::Vector3d::Ones(),
                    color.segment(0, 3), color[3]);
    }
}

}  // namespace lenny::gui