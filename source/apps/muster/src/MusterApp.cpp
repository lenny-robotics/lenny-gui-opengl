#include "MusterApp.h"

namespace lenny {

MusterApp::MusterApp() : gui::Application("MusterApp") {}

void MusterApp::restart() {}

void MusterApp::process() {}

void MusterApp::drawScene() const {}

void MusterApp::drawGui() {
    gui::Application::drawGui();
}

void MusterApp::mouseButtonCallback(double xPos, double yPos, int button, int action) {
    gui::Application::mouseButtonCallback(xPos, yPos, button, action);
}

void MusterApp::mouseMoveCallback(double xPos, double yPos) {
    gui::Application::mouseMoveCallback(xPos, yPos);
}

}  // namespace lenny