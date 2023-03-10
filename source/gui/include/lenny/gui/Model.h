#pragma once

#include <lenny/tools/Model.h>

#include <glm/glm.hpp>

namespace lenny::gui {

class Model : public tools::Model {
public:
    class Mesh {
    public:
        struct Vertex {
            glm::vec3 position = glm::vec3(0, 0, 0);
            glm::vec3 normal = glm::vec3(0, 0, 0);
            glm::vec2 texCoords = glm::vec2(0, 0);

            bool operator==(const Vertex &other) const {
                return position == other.position && normal == other.normal && texCoords == other.texCoords;
            }
        };

        struct Material {
            glm::vec3 ambient = glm::vec3(0.3f);
            glm::vec3 diffuse = glm::vec3(0.8f);
            glm::vec3 specular = glm::vec3(0.5f);

            std::optional<uint> texture_diffuse = std::nullopt;
        };

    public:
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint> &indices);
        Mesh(const std::vector<Vertex> &vertices, const std::vector<uint> &indices, const Material &material);
        ~Mesh() = default;

        void draw(const std::optional<Eigen::Vector3d> &color) const;

        const std::vector<Vertex>& getVertices() const;
        const std::vector<uint>& getIndices() const;
        const std::optional<Material>& getMaterial() const;

    private:
        void setup();

    private:
        std::vector<Vertex> vertices;
        std::vector<uint> indices;
        std::optional<Material> material;
        uint VAO, VBO, EBO;
    };

public:
    Model(const std::vector<Mesh> &meshes);
    Model(const std::string &filePath);
    ~Model() = default;

    static inline typename tools::Model::F_loadModel f_loadModel = [](tools::Model::UPtr &model, const std::string &filePath) -> void {
        model = std::make_unique<gui::Model>(filePath);
    };

    void draw(const Eigen::Vector3d &position, const Eigen::QuaternionD &orientation, const Eigen::Vector3d &scale, const std::optional<Eigen::Vector3d> &color,
              const double &alpha) const override;

    std::optional<HitInfo> hitByRay(const Eigen::Vector3d &position, const Eigen::QuaternionD &orientation, const Eigen::Vector3d &scale,
                                    const Ray &ray) const override;

    void load(const std::string &filePath);
    bool exportAsOBJ() const;
    void simplify(const float &threshold, const float &targetError, const bool &saveToFile);

public:
    std::vector<Mesh> meshes;
};

}  // namespace lenny::gui