#include <glad/glad.h>
#include <lenny/gui/Model.h>
#include <lenny/gui/Shaders.h>
#include <lenny/gui/Utils.h>
#include <lenny/tools/Utils.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/intersect.hpp>
#include <unordered_map>

namespace std {
template <>
struct hash<lenny::gui::Model::Mesh::Vertex> {
    size_t operator()(lenny::gui::Model::Mesh::Vertex const &vertex) const {
        return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoords) << 1);
    }
};
}  // namespace std

namespace lenny::gui {

Model::Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint> &indices) : vertices(vertices), indices(indices) {
    setup();
}

Model::Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint> &indices, const Material &material)
    : vertices(vertices), indices(indices), material(material) {
    setup();
}

void Model::Mesh::draw(const std::optional<Eigen::Vector3d> &color) const {
    //Update shader uniforms based on preferences
    Shaders::activeShader->setBool("useTexture", false);
    Shaders::activeShader->setBool("useMaterial", false);
    if (color.has_value()) {  //Use color
        Shaders::activeShader->setVec3("objectColor", utils::toGLM(color.value()));
    } else if (material.has_value() && material->texture_diffuse.has_value()) {                   //Use texture
        Shaders::activeShader->setBool("useTexture", true);                                       //Choose first texture from list
        glActiveTexture(GL_TEXTURE0);                                                             //Active proper texture unit before binding
        glUniform1i(glGetUniformLocation(Shaders::activeShader->getID(), "texture_diffuse"), 0);  //Set the sampler to the correct texture unit
        glBindTexture(GL_TEXTURE_2D, material->texture_diffuse.value());                          //Bind the texture
    } else if (material.has_value()) {                                                            //Use material
        Shaders::activeShader->setBool("useMaterial", true);
        Shaders::activeShader->setVec3("material.ambient", material.value().ambient);
        Shaders::activeShader->setVec3("material.diffuse", material.value().diffuse);
        Shaders::activeShader->setVec3("material.specular", material.value().specular);
    } else {  //Use default
        Shaders::activeShader->setVec3("objectColor", utils::toGLM(Eigen::Vector3d::Ones()));
    }

    //Draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Model::Mesh::setup() {
    //Create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Bind and load data
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

    //Set the vertex attribute pointers for ...
    //... positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)nullptr);

    //... normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    //... texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

    //Unbind array
    glBindVertexArray(0);
}

Model::Model(const std::string &filePath) : tools::Model(filePath) {
    load(filePath);
}

void Model::draw(const Eigen::Vector3d &position, const Eigen::QuaternionD &orientation, const Eigen::Vector3d &scale,
                 const std::optional<Eigen::Vector3d> &color, const double &alpha) const {
    Shaders::activeShader->activate();
    Shaders::activeShader->setMat4("modelPose", utils::getGLMTransform(position, orientation, scale));
    Shaders::activeShader->setFloat("objectAlpha", (float)alpha);
    for (const Mesh &mesh : meshes)
        mesh.draw(color);
}

std::optional<Model::HitInfo> Model::hitByRay(const Eigen::Vector3d &position, const Eigen::QuaternionD &orientation, const Eigen::Vector3d &scale,
                                              const Ray &ray) const {
    const glm::vec3 orig = utils::toGLM(ray.origin);
    const glm::vec3 dir = utils::toGLM(ray.direction);

    const glm::mat4 modelInv = glm::inverse(utils::getGLMTransform(position, orientation, scale));
    const glm::vec4 origModelTmp = modelInv * glm::vec4(orig, 1);
    const glm::vec3 origModel = glm::vec3(origModelTmp / origModelTmp.w);
    const glm::vec3 dirModel = glm::vec3(modelInv * glm::vec4(dir, 0));

    bool hit = false;
    double t = HUGE_VALF;
    Eigen::Vector3d hitPoint, hitNormal;

    for (const Mesh &m : meshes) {
        for (uint i = 0; i < m.indices.size() / 3; ++i) {
            glm::vec3 v0 = m.vertices[m.indices[3 * i + 0]].position;
            glm::vec3 v1 = m.vertices[m.indices[3 * i + 1]].position;
            glm::vec3 v2 = m.vertices[m.indices[3 * i + 2]].position;

            float t_ = 0.f;
            glm::vec2 bary;
            const bool tHit = glm::intersectRayTriangle(origModel, dirModel, v0, v1, v2, bary, t_);

            if (tHit && t_ > 1e-8 && t_ < t) {
                hit = true;
                t = t_;

                //Handle the scaling here, otherwise the normal is a bit messed up
                for (int idx = 0; idx < 3; idx++) {
                    v0[idx] *= scale[idx];
                    v1[idx] *= scale[idx];
                    v2[idx] *= scale[idx];
                }

                hitPoint = utils::toEigen(v0 * (1 - bary.x - bary.y) + v1 * bary.x + v2 * bary.y);
                hitNormal = utils::toEigen(v1 - v0).cross(utils::toEigen(v2 - v0)).normalized();
            }
        }
    }

    if (hit) {
        //The point is now in local coordinates, so switch it over to world coords...
        hitPoint = position + (orientation * hitPoint);
        hitNormal = orientation * hitNormal;
        return HitInfo{hitPoint, hitNormal, t};
    }
    return std::nullopt;
}

inline uint loadTextureFromFile(const std::string &fileName, const std::string &directory) {
    const std::string filePath = directory + '/' + fileName;

    uint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filePath.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        LENNY_LOG_WARNING("Failed to load texture from path `%s`", filePath.c_str());
    }
    stbi_image_free(data);

    return textureID;
}

void Model::load(const std::string &filePath) {
    //Check file extension
    const std::vector<std::string> supportedFileExtensions = {"obj", "OBJ", "stl", "STL", "dae", "DAE"};

    bool isSupportedFile = false;
    for (const std::string &extension : supportedFileExtensions) {
        if (tools::utils::checkFileExtension(filePath, extension)) {
            isSupportedFile = true;
            break;
        }
    }
    if (!isSupportedFile)
        LENNY_LOG_ERROR("File extension of `%s` is currently not supported", filePath.c_str());

    //--- Parse file
    Assimp::Importer importer;
    const uint loadFlags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices;
    const aiScene *pScene = importer.ReadFile(filePath.c_str(), loadFlags);
    if (!pScene)
        LENNY_LOG_ERROR("Error in parsing file `%s`: `%s`", filePath.c_str(), importer.GetErrorString());

    //--- Extract directory from filePath
    std::string tmpPath(filePath);
    std::replace(tmpPath.begin(), tmpPath.end(), '\\', '/');
    const std::string directory = tmpPath.substr(0, tmpPath.find_last_of('/'));

    //--- Materials
    std::vector<Mesh::Material> materials;
    for (uint i = 0; i < pScene->mNumMaterials; i++) {
        const aiMaterial *pMaterial = pScene->mMaterials[i];

        Mesh::Material material;
        aiColor3D aiC;

        //Ambient color
        if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, aiC) == AI_SUCCESS)
            material.ambient = {aiC.r, aiC.g, aiC.b};
        else
            LENNY_LOG_DEBUG("Ambient material color could not be read");

        //Diffuse color
        if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiC) == AI_SUCCESS)
            material.diffuse = {aiC.r, aiC.g, aiC.b};
        else
            LENNY_LOG_DEBUG("Diffuse material color could not be read");

        //Specular color
        if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, aiC) == AI_SUCCESS)
            material.specular = {aiC.r, aiC.g, aiC.b};
        else
            LENNY_LOG_DEBUG("Specular material color could not be read");

        //Texture
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString pPath;
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &pPath, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
                material.texture_diffuse = loadTextureFromFile(std::string(pPath.data), directory);
        }

        materials.emplace_back(material);
    }

    //--- Meshes
    this->meshes.clear();
    for (uint i = 0; i < pScene->mNumMeshes; i++) {
        const aiMesh *paiMesh = pScene->mMeshes[i];

        //Vertices
        std::vector<Mesh::Vertex> vertices;
        for (uint j = 0; j < paiMesh->mNumVertices; j++) {
            const aiVector3D &pPos = paiMesh->mVertices[j];

            aiVector3D pNor(0.f, 1.f, 0.f);
            if (paiMesh->mNormals)
                pNor = paiMesh->mNormals[j];
            else
                LENNY_LOG_DEBUG("No vertex normal available for file %s", filePath.c_str());

            aiVector3D tCoo(0.f, 0.f, 0.f);
            if (paiMesh->HasTextureCoords(0))
                tCoo = paiMesh->mTextureCoords[0][j];

            vertices.push_back({glm::vec3(pPos.x, pPos.y, pPos.z), glm::vec3(pNor.x, pNor.y, pNor.z), glm::vec2(tCoo.x, tCoo.y)});
        }

        //Indices
        std::vector<uint> indices;
        for (uint j = 0; j < paiMesh->mNumFaces; j++) {
            const aiFace &face = paiMesh->mFaces[j];

            if (face.mNumIndices == 3)
                for (uint k = 0; k < 3; k++)
                    indices.push_back(face.mIndices[k]);
            else
                LENNY_LOG_DEBUG("Number of indices should be 3, but instead is %d... We just ignore these indices", face.mNumIndices);
        }

        if (paiMesh->mMaterialIndex < materials.size())
            this->meshes.emplace_back(vertices, indices, materials[paiMesh->mMaterialIndex]);
        else
            this->meshes.emplace_back(vertices, indices);
    }
}

const std::vector<Model::Mesh> &Model::getMeshes() const {
    return this->meshes;
}

}  // namespace lenny::gui