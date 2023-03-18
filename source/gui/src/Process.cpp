#include <lenny/gui/ImGui.h>
#include <lenny/gui/Process.h>
#include <lenny/tools/Logger.h>
#include <lenny/tools/Timer.h>

namespace lenny::gui {

Process::Process(const std::string& description, const Function& f_process, const Function& f_restart, const bool& useSeparateThread)
    : description(description), f_process(f_process), f_restart(f_restart), useSeparateThread(useSeparateThread) {}

Process::~Process() {
    stop();
}

void Process::start() {
    if (processIsRunning || !f_process)
        return;
    processIsRunning = true;
    if (useSeparateThread)
        processThread = std::thread(&Process::run, this);
    LENNY_LOG_INFO("Process `%s` started", description.c_str())
}

void Process::stop() {
    if (!processIsRunning)
        return;
    processIsRunning = false;
    if (useSeparateThread)
        processThread.join();
    LENNY_LOG_INFO("Process `%s` terminated", description.c_str())
}

void Process::toggle() {
    processIsRunning ? stop() : start();
}

void Process::step() {
    if (f_process)
        f_process();
}

void Process::restart() {
    if (!f_restart)
        return;
    if (!processIsRunning) {
        f_restart();
    } else {
        stop();
        f_restart();
        start();
    }
    LENNY_LOG_INFO("Process `%s` restarted", description.c_str())
}

bool Process::separateThreadIsUsed() const {
    return useSeparateThread;
}

bool Process::isRunning() const {
    return processIsRunning;
}

double Process::getDt() const {
    return 1.0 / currentFramerate;
}

double Process::getCurrentFramerate() const {
    return currentFramerate;
}

void Process::drawGui() {
    if (ImGui::TreeNode(description.c_str())) {
        ImGui::Text("Run:");
        ImGui::SameLine();
        bool toggleProcess = processIsRunning;
        if (ImGui::ToggleButton("Run", &toggleProcess))
            toggleProcess ? start() : stop();

        if (!processIsRunning) {
            ImGui::Text("Step:");
            ImGui::SameLine();
            if (ImGui::ArrowButton("tmp", ImGuiDir_Right))
                step();
        }

        if (ImGui::Button("Restart"))
            restart();

        if (useSeparateThread) {
            ImGui::Checkbox("Limit FPS to", &limitFramerate);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50.f);
            ImGui::InputDouble(" ", &targetFramerate, 0.0, 0.0, "%.1f");
            if (processIsRunning) {
                static double drawFramerate = currentFramerate;
                static tools::Timer timer;
                if (timer.time() > 0.333) {
                    drawFramerate = currentFramerate;
                    timer.restart();
                }
                ImGui::Text("Current FPS: %.2f", drawFramerate);
            }
        }

        ImGui::TreePop();
    }
}

void Process::run() {
    tools::Timer timer;
    while (processIsRunning && f_process) {
        //Run function
        f_process();

        //Limit frame rate
        if (limitFramerate && (1.0 / targetFramerate) > timer.time())
            tools::Timer::sleep((1.0 / targetFramerate) - timer.time(), true);

        //Update current frame rate
        currentFramerate = 1.0 / timer.time();
        timer.restart();
    }
}

}  // namespace lenny::gui