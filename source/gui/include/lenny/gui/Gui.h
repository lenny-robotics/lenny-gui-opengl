#pragma once

#include <lenny/tools/Gui.h>

namespace lenny::gui {

class Gui : public tools::Gui {
public:
    //--- Constructor
    Gui() = default;
    ~Gui() = default;

    //--- Handling
    bool Begin(const char* name, bool* p_open = nullptr, int flags = 0) const override;
    void End() const override;

    bool TreeNode(const char* label) const override;
    void TreePop() const override;

    void PushItemWidth(float item_width) const override;
    void PopItemWidth() const override;

    void SameLine(float offset_from_start_x = 0.f, float spacing = -1.f) const override;
    void NewLine() const override;

    //--- Sliders
    bool Slider(const char* label, double& value, const double& min, const double& max, const char* format = nullptr, int flags = 0) const override;
    bool Slider(const char* label, float& value, const float& min, const float& max, const char* format = nullptr, int flags = 0) const override;
    bool Slider(const char* label, int& value, const int& min, const int& max, const char* format = nullptr, int flags = 0) const override;
    bool Slider(const char* label, uint& value, const uint& min, const uint& max, const char* format = nullptr, int flags = 0) const override;
    bool Slider(const char* label, Eigen::Vector2d& value, const Eigen::Vector2d& min, const Eigen::Vector2d& max, const char* format = nullptr,
                int flags = 0) const override;
    bool Slider(const char* label, Eigen::Vector3d& value, const Eigen::Vector3d& min, const Eigen::Vector3d& max, const char* format = nullptr,
                int flags = 0) const override;
    bool Slider(const char* label, Eigen::VectorXd& value, const Eigen::VectorXd& min, const Eigen::VectorXd& max, const char* format = nullptr,
                int flags = 0) const override;
    bool Slider(const char* label, Eigen::Matrix3d& value, const char* format = nullptr, int flags = 0) const override;
    bool Slider(const char* label, Eigen::QuaternionD& value, const char* format = nullptr, int flags = 0) const override;

    //--- Inputs
    bool Input(const char* label, double& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, float& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, int& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, uint& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, Eigen::Vector2d& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, Eigen::Vector3d& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, Eigen::VectorXd& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, Eigen::Matrix3d& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, Eigen::QuaternionD& value, const char* format = "%.6f", int flags = 0) const override;
    bool Input(const char* label, tools::Transformation& trafo, const char* format = "%.6f") const override;

    //--- Checkbox and buttons
    bool Checkbox(const char* label, bool& value) const override;

    bool Button(const char* label) const override;
    bool ToggleButton(const char* str_id, bool& v) const override;

    //--- Text
    void Text(const char* fmt, ...) const override;
    void TextColored(const Eigen::Vector4d& color, const char* fmt, ...) const override;

    //--- Color picker
    bool ColorPicker3(const char* label, Eigen::Vector3d& color) const override;
    bool ColorPicker4(const char* label, Eigen::Vector4d& color) const override;
};

}  // namespace lenny::gui