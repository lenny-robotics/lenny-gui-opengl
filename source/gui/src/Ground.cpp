#include <lenny/gui/Ground.h>
#include <lenny/gui/ImGui.h>
#include <lenny/gui/Renderer.h>
#include <lenny/tools/Logger.h>

namespace lenny::gui {

void Ground::drawScene() const {
    static const double alpha = 1.0;

    //Draw plane
    if (showPlane) {
        static const Eigen::Vector3d pos = Eigen::Vector3d::Zero();
        static const Eigen::QuaternionD ori = Eigen::QuaternionD::Identity();
        const Eigen::Vector2d dim = 2.0 * (double)size * Eigen::Vector2d::Ones();
        Eigen::Vector4d color;
        color << this->intensity * this->groundColor, alpha;
        Renderer::I->drawPlane(pos, ori, dim, color);
    }

    //Grid
    if (showGrid) {
        static const Eigen::QuaternionD ori = Eigen::QuaternionD::Identity();
        Eigen::Vector4d color;
        color << this->intensity * this->gridColor, alpha;
        for (int i = -size; i <= size; i++) {
            Renderer::I->drawCuboid(Eigen::Vector3d((double)i, 0.001, 0.0), ori, Eigen::Vector3d(gridThickness, 1e-3, (double)size * 2.0), color);
            Renderer::I->drawCuboid(Eigen::Vector3d(0.0, 0.001, (double)i), ori, Eigen::Vector3d((double)size * 2.0, 1e-3, gridThickness), color);
        }
    }
}

void Ground::drawGui() {
    if (ImGui::TreeNode("Ground")) {
        ImGui::Checkbox("Show Plane", &showPlane);
        ImGui::Checkbox("Show Grid", &showGrid);
        ImGui::SliderInt("Size", &size, 1, 100);
        ImGui::SliderDouble("Grid Thickness", &gridThickness, 0.001, 0.1);
        ImGui::SliderDouble("Intensity", &intensity, 0.0, 2.0);
        ImGui::ColorPicker3("Ground Color", groundColor);
        ImGui::ColorPicker3("Grid Color", gridColor);
        if (ImGui::Button("Print Settings"))
            printSettings();

        ImGui::TreePop();
    }
}

void Ground::printSettings() const {
    using tools::Logger;
    LENNY_LOG_PRINT(Logger::DEFAULT, "--- GROUND SETTINGS ---\n");
    LENNY_LOG_PRINT(Logger::DEFAULT, "ground.size = %d;\n", size);
    LENNY_LOG_PRINT(Logger::DEFAULT, "ground.gridThickness = %lf;\n", gridThickness);
    LENNY_LOG_PRINT(Logger::DEFAULT, "ground.intensity = %lf;\n", intensity);
    LENNY_LOG_PRINT(Logger::DEFAULT, "ground.groundColor = Eigen::Vector3d(%lf, %lf, %lf);\n", groundColor.x(), groundColor.y(), groundColor.z());
    LENNY_LOG_PRINT(Logger::DEFAULT, "ground.gridColor = Eigen::Vector3d(%lf, %lf, %lf);\n", gridColor.x(), gridColor.y(), gridColor.z());
    LENNY_LOG_PRINT(Logger::DEFAULT, "------------------------\n");
}

void Ground::to_json(json& j, const Ground& o) {
    TO_JSON(o, size)
    TO_JSON(o, gridThickness)
    TO_JSON(o, intensity)
    TO_JSON(o, groundColor)
    TO_JSON(o, gridColor)
    TO_JSON(o, showPlane)
    TO_JSON(o, showGrid)
}

void Ground::from_json(const json& j, Ground& o) {
    FROM_JSON(o, size)
    FROM_JSON(o, gridThickness)
    FROM_JSON(o, intensity)
    FROM_JSON(o, groundColor)
    FROM_JSON(o, gridColor)
    FROM_JSON(o, showPlane)
    FROM_JSON(o, showGrid)
}

}  // namespace lenny::gui