#pragma once

#include <implot.h>
#include <lenny/tools/Plot.h>

namespace lenny::gui {

template <typename T>
class Plot : public tools::Plot<T> {
public:
    //Constructor
    explicit Plot(const std::string &title, const std::string &xLabel, const std::string &yLabel, int maxSize = 1000)
        : tools::Plot<T>(title, xLabel, yLabel, maxSize) {}
    ~Plot() = default;

    //Function
    static inline typename tools::Plot<T>::F_addPlot f_addPlot = [](typename tools::Plot<T>::List &plots, const std::string &title, const std::string &xLabel,
                                                                    const std::string &yLabel, int maxSize = 1000) -> void {
        plots.emplace_back(std::make_unique<gui::Plot<T>>(title, xLabel, yLabel, maxSize));
    };

    //Draw plot
    void draw() override {
        //Settings
        ImGui::Checkbox(std::string("Always Fit X Axes##" + this->title).c_str(), &this->fitAxisX);
        ImGui::SameLine();
        ImGui::Checkbox(std::string("Always Fit Y Axes##" + this->title).c_str(), &this->fitAxisY);
        if (this->fitAxisX) {
            ImGui::SliderFloat(std::string("X Axis Scale##" + this->title).c_str(), &this->scaleAxisX, 0.1, 1);
            ImPlot::SetNextAxisLimits(ImAxis_X1, this->getXBegin() * this->scaleAxisX + this->getXEnd() * (1.f - this->scaleAxisX), this->getXEnd(),
                                      ImGuiCond_Always);
        }
        if (this->fitAxisY)
            ImPlot::SetNextAxisToFit(ImAxis_Y1);

        //Plot
        if (ImPlot::BeginPlot(this->title.c_str(), ImVec2(-1, 300))) {
            ImPlot::SetupAxes(this->xLabel.c_str(), this->yLabel.c_str());
            for (this->index = 0; this->index < this->lineSpecs.size(); this->index++) {
                ImPlot::SetNextMarkerStyle(this->lineSpecs[this->index].marker);

                if (this->lineSpecs[this->index].color.has_value()) {
                    const ImVec4 color(this->lineSpecs[this->index].color->at(0), this->lineSpecs[this->index].color->at(1),
                                       this->lineSpecs[this->index].color->at(2), 1.0);
                    ImPlot::SetNextLineStyle(color, 1.f);
                }

                auto getter_func = [](int idx, void *data) {
                    auto *my_data = (Plot<T> *)data;
                    auto &datapoint = my_data->getData(idx);
                    ImPlotPoint p;
                    p.x = (float)datapoint.first;
                    p.y = my_data->lineSpecs.at(my_data->index).getter(datapoint.second);
                    return p;
                };
                ImPlot::PlotLineG(this->lineSpecs[this->index].label.c_str(), getter_func, this, this->getSize());
            }

            ImPlot::EndPlot();
        }

        ImGui::Separator();
    }
};

}  // namespace lenny::gui