#pragma once

#include <lenny/tools/Typedefs.h>

namespace lenny::gui {

class Scene {
public:
    LENNY_GENERAGE_TYPEDEFS(Scene)

    Scene(const std::string& description, const int& width, const int& height);
    ~Scene();

    void draw();

public:  //ToDo: Private?
    float pos_x, pos_y;
    float size_x, size_y;
    const float width, height;
    const std::string description;
    uint frameBuffer;
    uint texture;
    uint depthRenderBuffer;
};

}  // namespace lenny::gui