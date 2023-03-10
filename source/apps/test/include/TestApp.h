#pragma once

#include <lenny/gui/Application.h>
#include <lenny/gui/Model.h>
#include <lenny/gui/Plot.h>

namespace lenny {

class TestApp : public gui::Application {
public:
    TestApp();
    ~TestApp() = default;

//    //--- Process
//    void restart() override;
//    void process() override;

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
        Model(const std::string& filePath, const double scale) : mesh(filePath), scale(scale * Eigen::Vector3d::Ones()) {}

        gui::Model mesh;
        Eigen::Vector3d position = Eigen::Vector3d::Random();
        Eigen::QuaternionD orientation = Eigen::QuaternionD::Identity();
        Eigen::Vector3d scale;
    };
    std::vector<Model> models = {Model(LENNY_GUI_TESTAPP_FOLDER "/config/nanosuit/NanoSuit.obj", 0.1),
                                 Model(LENNY_GUI_TESTAPP_FOLDER "/config/gripper/Gripper.obj", 3.0), Model(LENNY_GUI_TESTAPP_FOLDER "/config/car/Car.obj", 0.1),
                                 Model(LENNY_GUI_TESTAPP_FOLDER "/config/widowx/Base.stl", 0.003),
                                 Model(LENNY_GUI_TESTAPP_FOLDER "/config/spot/Body.dae", 1.0)};
    Model* selectedModel = nullptr;

    float data_x = 0.f;
    gui::Plot<Eigen::Vector3d> plot = gui::Plot<Eigen::Vector3d>("Plot", "x-Axis", "y-Axis", 1000);
};

}  // namespace lenny