#pragma once

#include <lenny/gui/Application.h>

namespace lenny {

class MusterApp : public gui::Application {
public:
    MusterApp();
    ~MusterApp() = default;

    //--- Process
    void restart() override;
    void process() override;

    //--- Drawing
    void drawScene() const override;
    void drawGui() override;

    //--- Interaction
    void mouseButtonCallback(double xPos, double yPos, int button, int action) override;
    void mouseMoveCallback(double xPos, double yPos) override;
};

}  // namespace lenny