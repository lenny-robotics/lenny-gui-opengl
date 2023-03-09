#pragma once

#include <lenny/tools/Json.h>

namespace lenny::gui {

class Ground {
public:
    Ground(int size = 10) : size(size) {}
    ~Ground() = default;

    void drawScene() const;
    void drawGui();

    void printSettings() const;
    static void to_json(json& j, const Ground& o);
    static void from_json(const json& j, Ground& o);

    void sync(const Ground& otherGround);

public:
    //Sizes
    int size;  //Set by constructor
    double gridThickness = 0.025;

    //Color
    double intensity = 1.2;
    Eigen::Vector3d groundColor = Eigen::Vector3d(1.0, 1.0, 1.0);
    Eigen::Vector3d gridColor = Eigen::Vector3d(0.548544, 0.548544, 0.548544);

    //Bools
    bool showPlane = true;
    bool showGrid = true;
};

}  // namespace lenny::gui