#pragma once

#include <lenny/gui/Application.h>
#include <lenny/gui/Plot.h>
#include <lenny/tools/Trajectory.h>

namespace lenny {

class InterpolationApp : public gui::Application {
public:
    InterpolationApp();
    ~InterpolationApp() = default;

    void drawGui() override;

public:
    tools::Trajectory1d trajectory;
    gui::Plot<double> linearPlot = gui::Plot<double>("Linear Plot", "x-Axis", "y-Axis", 1000);
    gui::Plot<double> splinePlot = gui::Plot<double>("Spline Plot", "x-Axis", "y-Axis", 1000);
};

}  // namespace lenny