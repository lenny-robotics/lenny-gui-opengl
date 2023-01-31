#include "TestApp.h"

#include <lenny/gui/ImGui.h>
#include <lenny/gui/ImGuizmo.h>
#include <lenny/gui/Renderer.h>
#include <lenny/tools/Logger.h>

namespace lenny {

TestApp::TestApp() : gui::Application("TestApp") {
    plot.addLineSpec({"x", [](const Eigen::Vector3d& d) { return (float)d.x(); }});
    plot.addLineSpec({"y", [](const Eigen::Vector3d& d) { return (float)d.y(); }});
    plot.addLineSpec({"z", [](const Eigen::Vector3d& d) { return (float)d.z(); }});
}

void TestApp::restart() {
    consoleIter = 0;
    data_x = 0.f;
}

void TestApp::process() {
    LENNY_LOG_INFO("Test: %d", consoleIter++);
    plot.addData(data_x, Eigen::Vector3d::Random());
    data_x += (float)getDt();
}

void TestApp::drawScene() const {
    //--- Renderer
    if (showRenderings) {
        auto randomDim = []() -> Eigen::Vector3d {
            Eigen::Vector3d vec;
            for (int i = 0; i < 3; i++)
                vec[i] = tools::utils::getRandomNumberInRange({0.1, 1.0});
            return vec;
        };

        {
            static const Eigen::Vector3d COM = Eigen::Vector3d::Random();
            static const Eigen::QuaternionD orientation = Eigen::QuaternionD::UnitRandom();
            static const Eigen::Vector3d dimensions = randomDim();
            gui::Renderer::I->drawCuboid(COM, orientation, dimensions, rendererColor);
        }

        {
            static const Eigen::Vector3d position = Eigen::Vector3d::Random();
            static const double radius = tools::utils::getRandomNumberInRange({0.1, 0.5});
            gui::Renderer::I->drawSphere(position, radius, rendererColor);
        }

        {
            static const Eigen::Vector3d COM = Eigen::Vector3d::Random();
            static const Eigen::QuaternionD orientation = Eigen::QuaternionD::UnitRandom();
            static const Eigen::Vector3d dimensions = randomDim();
            gui::Renderer::I->drawEllipsoid(COM, orientation, dimensions, rendererColor);
        }

        {
            static const Eigen::Vector3d COM = Eigen::Vector3d::Random();
            static const Eigen::QuaternionD orientation = Eigen::QuaternionD::UnitRandom();
            static const double radius = tools::utils::getRandomNumberInRange({0.01, 0.1});
            static const double height = tools::utils::getRandomNumberInRange({0.2, 0.5});
            gui::Renderer::I->drawCapsule(COM, orientation, height, radius, rendererColor);
        }

        {
            static std::vector<Eigen::Vector3d> points = {Eigen::Vector3d::Random(), Eigen::Vector3d::Random(), Eigen::Vector3d::Random(),
                                                          Eigen::Vector3d::Random(), Eigen::Vector3d::Random()};
            static const double radius = tools::utils::getRandomNumberInRange({0.01, 0.1});
            gui::Renderer::I->drawTrajectory(points, radius, rendererColor, true);
        }

        {
            static const Eigen::Vector3d center = Eigen::Vector3d::Random();
            static const Eigen::QuaternionD orientation = Eigen::QuaternionD::UnitRandom();
            static const double radius = tools::utils::getRandomNumberInRange({0.1, 0.1});
            gui::Renderer::I->drawSector(center, orientation, radius, std::pair<double, double>{0.0, 1.5 * PI}, rendererColor);
        }
    }

    //--- Models
    std::optional<Eigen::Vector3d> modelColor = std::nullopt;
    if (!showMaterials)
        modelColor = rendererColor.segment(0, 3);
    for (const Model& model : models)
        model.mesh.draw(model.position, model.orientation, model.scale, modelColor, rendererColor[3]);
}

void TestApp::drawGui() {
    //--- ImGui
    gui::Application::drawGui();

    ImGui::Begin("Main Menu");

    ImGui::ColorPicker4("Renderer Color", rendererColor);
    ImGui::Checkbox("Show Materials", &showMaterials);

    //--- ImPlot
    if (ImGui::TreeNode("Plot")) {
        plot.draw();
        ImGui::TreePop();
    }

    //--- ImGuizmo
//    selectedModel = &models.at(0);
//    if (selectedModel) {
//        ImGui::SetNextItemOpen(true);
//        ImGui::Begin("ImGuizmo");
//        ImGuizmo::useWidget(selectedModel->position, selectedModel->orientation, selectedModel->scale, scenes.at(0)->camera.getViewMatrix(),
//                            scenes.at(0)->camera.getProjectionMatrix());
//
//        static float threshold = 0.8f;
//        static float targetError = 0.01;
//        static bool saveToFile = false;
//        ImGui::SliderFloat("Threshold", &threshold, 0.f, 1.f);
//        ImGui::SliderFloat("Target Error", &targetError, 0.f, 1.f);
//        ImGui::Checkbox("Save To File", &saveToFile);
//        if (ImGui::Button("Simplify"))
//            selectedModel->mesh.simplify(threshold, targetError, saveToFile);
//
//        if (ImGui::Button("Export as OBJ"))
//            selectedModel->mesh.exportAsOBJ();
//
//        ImGui::End();
//    }

    ImGui::End();
}

void TestApp::mouseButtonCallback(double xPos, double yPos, int button, int action) {
//    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//        const auto ray = scenes.at(0)->camera.getRayFromScreenCoordinates(xPos, yPos);
//        selectedModel = nullptr;
//        for (Model& model : models) {
//            const auto hitInfo = model.mesh.hitByRay(model.position, model.orientation, model.scale, ray);
//            if (hitInfo.has_value()) {
//                selectedModel = &model;
//                break;
//            }
//        }
//    }
    gui::Application::mouseButtonCallback(xPos, yPos, button, action);
}

void TestApp::fileDropCallback(int count, const char** fileNames) {
    models.emplace_back(fileNames[count - 1], 1.0);
}

}  // namespace lenny
