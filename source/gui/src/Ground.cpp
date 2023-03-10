#include <lenny/gui/Ground.h>
#include <lenny/gui/ImGui.h>
#include <lenny/tools/Logger.h>

namespace lenny::gui {

Ground::Ground(int size) {
    setSize(size);
}

void Ground::setSize(const int size) {
    this->size = size;

    //Reset model
    std::vector<Model::Mesh::Vertex> vertices;
    std::vector<uint> indices;
    Model::Mesh::Material material = model.meshes.back().getMaterial().value();
    for (int i = -size; i < size; i++) {
        for (int j = -size; j < size; j++) {
            for(const auto& index : tile.meshes.back().getIndices()) {
                indices.emplace_back(index + (uint)vertices.size());
            }
            for(const auto& vertex : tile.meshes.back().getVertices()) {
                vertices.emplace_back(vertex);
                vertices.back().position += glm::vec3((double)i + 0.5, 0.0, (double)j + 0.5);
            }
        }
    }
    model.meshes.back() = Model::Mesh(vertices, indices, material);
}


void Ground::drawScene() const {
    model.draw(Eigen::Vector3d::Zero(), Eigen::QuaternionD::Identity(), Eigen::Vector3d::Ones(), std::nullopt, alpha);
}

void Ground::drawGui() {
    if (ImGui::TreeNode("Ground")) {
        if(ImGui::SliderInt("Size", &size, 1, 100))
            setSize(size);
        ImGui::SliderDouble("Alpha", &alpha, 0.0, 1.0);
        if (ImGui::Button("Print Settings"))
            printSettings();

        ImGui::TreePop();
    }
}

void Ground::printSettings() const {
    using tools::Logger;
    LENNY_LOG_PRINT(Logger::DEFAULT, "--- GROUND SETTINGS ---\n");
    LENNY_LOG_PRINT(Logger::DEFAULT, "ground.size = %d;\n", size);
    LENNY_LOG_PRINT(Logger::DEFAULT, "ground.alpha = %lf;\n", alpha);
    LENNY_LOG_PRINT(Logger::DEFAULT, "------------------------\n");
}

void Ground::to_json(json& j, const Ground& o) {
    TO_JSON(o, size)
    TO_JSON(o, alpha)
}

void Ground::from_json(const json& j, Ground& o) {
    FROM_JSON(o, size)
    FROM_JSON(o, alpha)
}

void Ground::sync(const Ground& otherGround) {
    this->size = otherGround.size;
    this->alpha = otherGround.alpha;
}

}  // namespace lenny::gui