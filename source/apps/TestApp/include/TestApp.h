#pragma once

#include <lenny/gui/Application.h>
#include <lenny/gui/Model.h>
#include <lenny/gui/Plot.h>

namespace lenny {

class TestApp : public gui::Application {
public:
    TestApp();
    ~TestApp() = default;

    //--- Process
    void restart();
    void process();

    //--- Drawing
    void drawScene() const;
    void drawGui() override;
    void drawGuizmo() override;

    //--- Interaction
    void mouseButtonCallback(double xPos, double yPos, Ray ray, int button, int action);
    void fileDropCallback(int count, const char** fileNames);

public:
    int consoleIter = 0;
    Eigen::Vector4d rendererColor = Eigen::Vector4d(0.75, 0.75, 0.75, 1.0);
    bool showRenderings = false;
    bool showMaterials = true;

    struct Model {
        Model(const std::string& filePath, const Eigen::Vector3d& position, const Eigen::QuaternionD& orientation, const double& scale)
            : mesh(filePath), position(position), orientation(orientation), scale(scale * Eigen::Vector3d::Ones()) {}

        gui::Model mesh;
        Eigen::Vector3d position;
        Eigen::QuaternionD orientation;
        Eigen::Vector3d scale;
    };
    std::vector<Model> models = {
        Model(LENNY_GUI_TESTAPP_FOLDER "/config/yumi/Base.obj", Eigen::Vector3d(-1.0, 0.5, 0.0),
              Eigen::QuaternionD(tools::utils::rotY(-PI / 2.0) * tools::utils::rotX(-PI / 2.0)), 1.0),
        Model(LENNY_GUI_TESTAPP_FOLDER "/config/gripper/Gripper.obj", Eigen::Vector3d(-0.5, 0.5, 0.0), Eigen::QuaternionD::Identity(), 3.0),
        Model(LENNY_GUI_TESTAPP_FOLDER "/config/nao/12211_Robot_l2.obj", Eigen::Vector3d(0.0, 0.5, 0.0), Eigen::QuaternionD(tools::utils::rotX(-PI / 2.0)),
              0.03),
        Model(LENNY_GUI_TESTAPP_FOLDER "/config/widowx/Base.stl", Eigen::Vector3d(0.5, 0.5, 0.0), Eigen::QuaternionD(tools::utils::rotX(-PI / 2.0)), 0.003),
        Model(LENNY_GUI_TESTAPP_FOLDER "/config/spot/Body.dae", Eigen::Vector3d(1.0, 0.5, 0.0), Eigen::QuaternionD::Identity(), 1.0)};
    Model* selectedModel = nullptr;

    float data_x = 0.f;
    gui::Plot<Eigen::Vector3d> plot = gui::Plot<Eigen::Vector3d>("Plot", "x-Axis", "y-Axis", 1000);
};

}  // namespace lenny