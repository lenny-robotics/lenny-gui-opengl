#pragma once

#include <imgui.h>
#include <lenny/tools/Transformation.h>

namespace ImGui {

IMGUI_API bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format = nullptr, ImGuiSliderFlags flags = 0);
IMGUI_API bool InputUInt(const char* label, unsigned int* v, unsigned int step = 1, unsigned int step_fast = 100, ImGuiInputTextFlags flags = 0);
IMGUI_API bool SliderUInt(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);

template <int Size>
bool InputVectorD(const char* label, Eigen::Matrix<double, Size, 1>& v, const char* format = "%.6f", ImGuiInputTextFlags flags = 0) {
    bool triggered = false;
    if (ImGui::TreeNode(label)) {
        for (int i = 0; i < (int)v.size(); i++) {
            if (ImGui::InputDouble(std::to_string(i).c_str(), &v[i], 0.0, 0.0, format, flags))
                triggered = true;
        }
        ImGui::TreePop();
    }
    return triggered;
}

template <int Size>
bool SliderVectorD(const char* label, Eigen::Matrix<double, Size, 1>& v, const Eigen::Matrix<double, Size, 1>& v_min,
                   const Eigen::Matrix<double, Size, 1>& v_max, const char* format = "%.6f", ImGuiSliderFlags flags = 0) {
    bool triggered = false;
    if (ImGui::TreeNode(label)) {
        for (int i = 0; i < (int)v.size(); i++) {
            if (ImGui::SliderDouble(std::to_string(i).c_str(), &v[i], v_min[i], v_max[i], format, flags))
                triggered = true;
        }
        ImGui::TreePop();
    }
    return triggered;
}

bool SliderOrientation(const char* label, Eigen::Matrix3d& R, const char* format = "%.6f", ImGuiSliderFlags flags = 0);
bool SliderOrientation(const char* label, Eigen::QuaternionD& q, const char* format = "%.6f", ImGuiSliderFlags flags = 0);

bool InputOrientation(const char* label, Eigen::Matrix3d& R, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
bool InputOrientation(const char* label, Eigen::QuaternionD& q, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
bool InputTransformation(const char* label, lenny::tools::Transformation& trafo, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);

bool ColorPicker3(const char* label, Eigen::Vector3d& color);
bool ColorPicker4(const char* label, Eigen::Vector4d& color);

bool ToggleButton(const char* str_id, bool* v);

}  // namespace ImGui
