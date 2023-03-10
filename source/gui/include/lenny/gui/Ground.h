#pragma once

#include <lenny/tools/Json.h>
#include <lenny/gui/Model.h>

namespace lenny::gui {

class Ground {
public:
    Ground(int size = 50);
    ~Ground() = default;

    void setSize(const int size);

    void drawScene() const;
    void drawGui();

    void printSettings() const;
    static void to_json(json& j, const Ground& o);
    static void from_json(const json& j, Ground& o);

    void sync(const Ground& otherGround);

public:
    double alpha = 1.0;

private:
    int size; //Set by constructor
    const Model tile = Model(LENNY_GUI_OPENGL_FOLDER "/data/ground/ground.obj");
    Model model = Model(tile.meshes);
};

}  // namespace lenny::gui