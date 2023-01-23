#include <imgui_internal.h>
#include <lenny/gui/ImGui.h>
#include <lenny/tools/EulerAngleRigidBody.h>

namespace ImGui {

IMGUI_API bool SliderDouble(const char* label, double* v, double v_min, double v_max, const char* format, ImGuiSliderFlags flags) {
    return ImGui::SliderScalar(label, ImGuiDataType_Double, v, &v_min, &v_max, format, flags);
}

IMGUI_API bool InputUInt(const char* label, unsigned int* v, unsigned int step, unsigned int step_fast, ImGuiInputTextFlags flags) {
    const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
    return ImGui::InputScalar(label, ImGuiDataType_U32, (void*)v, (void*)(step > 0 ? &step : nullptr), (void*)(step_fast > 0 ? &step_fast : nullptr), format,
                              flags);
}

IMGUI_API bool SliderUInt(const char* label, unsigned int* v, unsigned int v_min, unsigned int v_max, const char* format, ImGuiSliderFlags flags) {
    return ImGui::SliderScalar(label, ImGuiDataType_U32, v, &v_min, &v_max, format, flags);
}

bool SliderOrientation(const char* label, Eigen::Matrix3d& R, const char* format, ImGuiSliderFlags flags) {
    Eigen::QuaternionD q(R);
    const bool triggered = SliderOrientation(label, q, format, flags);
    R = q.matrix();
    return triggered;
}

bool SliderOrientation(const char* label, Eigen::QuaternionD& q, const char* format, ImGuiSliderFlags flags) {
    static lenny::tools::EulerAngleRigidBody rb;
    Eigen::Vector6d state = rb.getStateFromTransformation(lenny::tools::Transformation(Eigen::Vector3d::Zero(), q));
    bool triggered = false;
    if (ImGui::TreeNode(label)) {
        for (int i = 0; i < 3; i++) {
            if (ImGui::SliderDouble(std::to_string(i).c_str(), &state[i + 3], -2.0 * PI, 2.0 * PI, format, flags))
                triggered = true;
        }
        q = rb.getTransformationFromState(state).orientation;
        ImGui::Text("w: %lf, x: %lf, y: %lf, z: %lf", q.w(), q.x(), q.y(), q.z());
        ImGui::TreePop();
    }
    return triggered;
}

bool InputOrientation(const char* label, Eigen::Matrix3d& R, const char* format, ImGuiInputTextFlags flags) {
    Eigen::QuaternionD q(R);
    const bool triggered = InputOrientation(label, q, format, flags);
    R = q.matrix();
    return triggered;
}

bool InputOrientation(const char* label, Eigen::QuaternionD& q, const char* format, ImGuiInputTextFlags flags) {
    static lenny::tools::EulerAngleRigidBody rb;
    Eigen::Vector6d state = rb.getStateFromTransformation(lenny::tools::Transformation(Eigen::Vector3d::Zero(), q));
    bool triggered = false;
    if (ImGui::TreeNode(label)) {
        for (int i = 0; i < 3; i++) {
            if (ImGui::InputDouble(std::to_string(i).c_str(), &state[i + 3], 0.0, 0.0, format, flags))
                triggered = true;
        }
        q = rb.getTransformationFromState(state).orientation;
        ImGui::Text("w: %lf, x: %lf, y: %lf, z: %lf", q.w(), q.x(), q.y(), q.z());
        ImGui::TreePop();
    }
    return triggered;
}

bool InputTransformation(const char* label, lenny::tools::Transformation& trafo, const char* format, ImGuiInputTextFlags flags) {
    bool trig_pos = false;
    bool trig_rot = false;
    if (ImGui::TreeNode(label)) {
        trig_pos = InputVectorD<3>("Position", trafo.position, format, flags);
        trig_rot = InputOrientation("Orientation", trafo.orientation, format, flags);

        ImGui::TreePop();
    }
    return trig_pos || trig_rot;
}

bool ColorPicker3(const char* label, Eigen::Vector3d& color) {
    bool manipulated = false;
    if (ImGui::TreeNode(("ColorPicker - " + std::string(label)).c_str())) {
        float col[3] = {(float)color[0], (float)color[1], (float)color[2]};
        if (ImGui::ColorPicker3(label, &col[0])) {
            for (int i = 0; i < 3; i++) {
                color[i] = (double)col[i];
            }
            manipulated = true;
        }
        ImGui::TreePop();
    }
    return manipulated;
}

bool ColorPicker4(const char* label, Eigen::Vector4d& color) {
    bool manipulated = false;
    if (ImGui::TreeNode(("ColorPicker - " + std::string(label)).c_str())) {
        float col[4] = {(float)color[0], (float)color[1], (float)color[2], (float)color[3]};
        if (ImGui::ColorPicker4(label, &col[0])) {
            for (int i = 0; i < 4; i++) {
                color[i] = (double)col[i];
            }
            manipulated = true;
        }
        ImGui::TreePop();
    }
    return manipulated;
}

bool ToggleButton(const char* str_id, bool* v) {
    bool clicked = false;
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    ImGui::InvisibleButton(str_id, ImVec2(width, height));
    if (ImGui::IsItemClicked()) {
        clicked = true;
        *v = !*v;
    }

    float t = *v ? 1.0f : 0.0f;

    ImGuiContext& g = *GImGui;
    float ANIM_SPEED = 0.08f;
    if (g.LastActiveId == g.CurrentWindow->GetID(str_id))  // && g.LastActiveIdTimer < ANIM_SPEED)
    {
        float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
        t = *v ? (t_anim) : (1.0f - t_anim);
    }

    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
    else
        col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));

    return clicked;
}

}  // namespace ImGui