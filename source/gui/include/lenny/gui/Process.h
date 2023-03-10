#pragma once

#include <functional>
#include <thread>
#include <string>
#include <lenny/tools/Typedefs.h>

namespace lenny::gui {

class Process {
public:
    LENNY_GENERAGE_TYPEDEFS(Process)
    typedef std::function<void()> Function;
    Process(const std::string& description, const Function& f_process, const Function& f_restart);
    ~Process();

    void start();
    void stop();
    void toggle();
    void step();
    void restart();

    bool isRunning() const;
    double getDt() const;
    double getCurrentFramerate() const;

    void drawGui();

private:
    void run();

public:
    const std::string description;  //Set by constructor
    bool limitFramerate = true;
    double targetFramerate = 100.0;

private:
    Function f_process, f_restart;  //Set by constructor
    double currentFramerate = targetFramerate;
    bool processIsRunning = false;
    std::thread processThread;
};
}  // namespace lenny::gui