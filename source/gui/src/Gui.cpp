#include <lenny/gui/Gui.h>
#include <lenny/gui/ImGui.h>

namespace lenny::gui {

bool Gui::Begin(const char* name, bool* p_open, int flags) const {
    return ImGui::Begin(name, p_open, flags);
}

void Gui::End() const {
    ImGui::End();
}

bool Gui::TreeNode(const char* label) const {
    return ImGui::TreeNode(label);
}

void Gui::TreePop() const {
    ImGui::TreePop();
}

void Gui::PushItemWidth(float item_width) const {
    ImGui::PushItemWidth(item_width);
}

void Gui::PopItemWidth() const {
    ImGui::PopItemWidth();
}

void Gui::SameLine(float offset_from_start_x, float spacing) const {
    ImGui::SameLine(offset_from_start_x, spacing);
}

void Gui::NewLine() const {
    ImGui::NewLine();
}

bool Gui::Slider(const char* label, double& value, const double& min, const double& max, const char* format, int flags) const {
    return ImGui::SliderDouble(label, &value, min, max, format, flags);
}

bool Gui::Slider(const char* label, float& value, const float& min, const float& max, const char* format, int flags) const {
    return ImGui::SliderFloat(label, &value, min, max, format, flags);
}

bool Gui::Slider(const char* label, int& value, const int& min, const int& max, const char* format, int flags) const {
    return ImGui::SliderInt(label, &value, min, max, format, flags);
}

bool Gui::Slider(const char* label, uint& value, const uint& min, const uint& max, const char* format, int flags) const {
    return ImGui::SliderUInt(label, &value, min, max, format, flags);
}

bool Gui::Slider(const char* label, Eigen::Vector2d& value, const Eigen::Vector2d& min, const Eigen::Vector2d& max, const char* format, int flags) const {
    return ImGui::SliderVectorD<2>(label, value, min, max, format, flags);
}

bool Gui::Slider(const char* label, Eigen::Vector3d& value, const Eigen::Vector3d& min, const Eigen::Vector3d& max, const char* format, int flags) const {
    return ImGui::SliderVectorD<3>(label, value, min, max, format, flags);
}

bool Gui::Slider(const char* label, Eigen::VectorXd& value, const Eigen::VectorXd& min, const Eigen::VectorXd& max, const char* format, int flags) const {
    return ImGui::SliderVectorD<-1>(label, value, min, max, format, flags);
}

bool Gui::Slider(const char* label, Eigen::Matrix3d& value, const char* format, int flags) const {
    return ImGui::SliderOrientation(label, value, format, flags);
}

bool Gui::Slider(const char* label, Eigen::QuaternionD& value, const char* format, int flags) const {
    return ImGui::SliderOrientation(label, value, format, flags);
}

bool Gui::Input(const char* label, double& value, const char* format, int flags) const {
    return ImGui::InputDouble(label, &value, 0.0, 0.0, format, flags);
}

bool Gui::Input(const char* label, float& value, const char* format, int flags) const {
    return ImGui::InputFloat(label, &value, 0.f, 0.f, format, flags);
}

bool Gui::Input(const char* label, int& value, const char* format, int flags) const {
    return ImGui::InputInt(label, &value, 1, 100, flags);
}

bool Gui::Input(const char* label, uint& value, const char* format, int flags) const {
    return ImGui::InputUInt(label, &value, 1, 100, flags);
}

bool Gui::Input(const char* label, Eigen::Vector2d& value, const char* format, int flags) const {
    return ImGui::InputVectorD<2>(label, value, format);
}

bool Gui::Input(const char* label, Eigen::Vector3d& value, const char* format, int flags) const {
    return ImGui::InputVectorD<3>(label, value, format);
}

bool Gui::Input(const char* label, Eigen::VectorXd& value, const char* format, int flags) const {
    return ImGui::InputVectorD<-1>(label, value, format);
}

bool Gui::Input(const char* label, Eigen::Matrix3d& value, const char* format, int flags) const {
    return ImGui::InputOrientation(label, value, format, flags);
}

bool Gui::Input(const char* label, Eigen::QuaternionD& value, const char* format, int flags) const {
    return ImGui::InputOrientation(label, value, format, flags);
}

bool Gui::Input(const char* label, tools::Transformation& trafo, const char* format) const {
    return ImGui::InputTransformation(label, trafo, format);
}

bool Gui::Checkbox(const char* label, bool& value) const {
    return ImGui::Checkbox(label, &value);
}

bool Gui::Button(const char* label) const {
    return ImGui::Button(label);
}

bool Gui::ToggleButton(const char* str_id, bool& v) const {
    return ImGui::ToggleButton(str_id, &v);
}

void Gui::Text(const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    ImGui::TextV(fmt, args);
    va_end(args);
}

void Gui::TextColored(const Eigen::Vector4d& color, const char* fmt, ...) const {
    va_list args;
    va_start(args, fmt);
    ImGui::TextColoredV(ImVec4((float)color[0], (float)color[1], (float)color[2], (float)color[3]), fmt, args);
    va_end(args);
}

bool Gui::ColorPicker3(const char* label, Eigen::Vector3d& color) const {
    return ImGui::ColorPicker3(label, color);
}

bool Gui::ColorPicker4(const char* label, Eigen::Vector4d& color) const {
    return ImGui::ColorPicker4(label, color);
}

}  // namespace lenny::gui
