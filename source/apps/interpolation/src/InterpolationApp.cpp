#include "InterpolationApp.h"

namespace lenny {

InterpolationApp::InterpolationApp() : gui::Application("InterpolationApp") {
    showGround = false;
    showOrigin = false;
    showConsole = false;

    trajectory.addEntry(-2.0, 3.0);
    trajectory.addEntry(-1.0, 1.0);
    trajectory.addEntry(1.0, 1.0);
    trajectory.addEntry(2.0, -1.0);

    linearPlot.addLineSpec({"x", [](const double& d) { return (float)d; }, std::array<float, 3>{1.f, 0.f, 0.f}, ImPlotMarker_None});
    splinePlot.addLineSpec({"x", [](const double& d) { return (float)d; }, std::array<float, 3>{0.f, 1.f, 0.f}, ImPlotMarker_None});
    for (double t = -3.0; t <= 3.0; t += 0.1) {
        linearPlot.addData((float)t, trajectory.getLinearInterpolation(t));
        splinePlot.addData((float)t, trajectory.getSplineInterpolation(t));
    }
}

void InterpolationApp::drawGui() {
    linearPlot.draw();
    splinePlot.draw();
}

}  // namespace lenny