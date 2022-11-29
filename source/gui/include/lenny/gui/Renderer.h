#pragma once

#include <lenny/tools/Renderer.h>

namespace lenny::gui {

class Renderer : public tools::Renderer {
public:
    //--- Constructor
    Renderer() = default;
    ~Renderer() = default;

    //--- Draw functions
    void drawCuboid(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const Eigen::Vector3d& dimensions,
                    const Eigen::Vector4d& color) const override;
    void drawPlane(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const Eigen::Vector2d& dimensions,
                   const Eigen::Vector4d& color) const override;
    void drawSphere(const Eigen::Vector3d& position, const double& radius, const Eigen::Vector4d& color) const override;
    void drawEllipsoid(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const Eigen::Vector3d& dimensions,
                       const Eigen::Vector4d& color) const override;
    void drawCylinder(const Eigen::Vector3d& startPosition, const Eigen::Vector3d& endPosition, const double& radius,
                      const Eigen::Vector4d& color) const override;
    void drawCylinder(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const double& height, const double& radius,
                      const Eigen::Vector4d& color) const override;
    void drawCone(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction, const double& radius, const Eigen::Vector4d& color) const override;
    void drawArrow(const Eigen::Vector3d& startPosition, const Eigen::Vector3d& direction, const double& radius, const Eigen::Vector4d& color) const override;
    void drawCoordinateSystem(const Eigen::Vector3d& origin, const Eigen::QuaternionD& orientation, const double& length, const double& radius,
                              double alpha = 1.0) const override;
    void drawCapsule(const Eigen::Vector3d& startPosition, const Eigen::Vector3d& endPosition, const double& radius,
                     const Eigen::Vector4d& color) const override;
    void drawCapsule(const Eigen::Vector3d& COM, const Eigen::QuaternionD& orientation, const double& height, const double& radius,
                     const Eigen::Vector4d& color) const override;
    void drawTetrahedron(const std::array<Eigen::Vector3d, 4>& globalPoints, const Eigen::Vector4d& color) const override;
    void drawLine(const std::vector<Eigen::Vector3d>& linePoints, const double& radius, const Eigen::Vector4d& color) const override;
    void drawTrajectory(const std::vector<Eigen::Vector3d>& trajectoryPoints, const double& radius, const Eigen::Vector4d& color,
                        const bool& showDots) const override;
    void drawSector(const Eigen::Vector3d& center, const Eigen::QuaternionD& orientation, const double& radius, const std::pair<double, double>& angleRange,
                    const Eigen::Vector4d& color) const override;
};

}  // namespace lenny::gui