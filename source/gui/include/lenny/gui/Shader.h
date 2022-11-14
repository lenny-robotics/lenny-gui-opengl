#pragma once

#include <glm/glm.hpp>
#include <string>

namespace lenny::gui {

class Shader {
public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);
    ~Shader() = default;

    void activate() const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    unsigned int getID() const;

private:
    void load(const std::string &vertexPath, const std::string &fragmentPath);
    void getCodeFromFile(std::string &code, const std::string &path) const;

    void checkShaderCompilationErrors(const unsigned int shader, const std::string& type) const;
    void checkProgramCompilationErrors(const unsigned int program) const;

private:
    unsigned int ID;  //Set in load function
};

}  // namespace lenny::gui