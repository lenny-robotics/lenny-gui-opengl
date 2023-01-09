#include <glad/glad.h>
#include <lenny/gui/Shader.h>
#include <lenny/tools/Logger.h>

#include <fstream>
#include <sstream>
#include <vector>

namespace lenny::gui {

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath) {
    load(vertexPath, fragmentPath);
}

void Shader::activate() const {
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

unsigned int Shader::getID() const {
    return ID;
}

void Shader::load(const std::string &vertexPath, const std::string &fragmentPath) {
    //--- Retrieve the vertex/fragment source code from the individual files
    std::string vertexCode, fragmentCode;
    getCodeFromFile(vertexCode, vertexPath);
    getCodeFromFile(fragmentCode, fragmentPath);
    const char *vCode = vertexCode.c_str();
    const char *fCode = fragmentCode.c_str();

    //--- Compile shaders
    unsigned int vertex, fragment;

    //Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, nullptr);
    glCompileShader(vertex);
    checkShaderCompilationErrors(vertex, "VERTEX");

    //Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, nullptr);
    glCompileShader(fragment);
    checkShaderCompilationErrors(fragment, "FRAGMENT");

    //--- Link program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkProgramCompilationErrors(ID);

    // --- Delete shaders
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::getCodeFromFile(std::string &code, const std::string &path) const {
    //Implement split function
    auto split = [](std::vector<std::string> &v, const std::string &s, const char delim) -> void {
        v.clear();
        auto p = std::begin(s);
        for (auto q = std::find(p, std::end(s), delim); q != std::end(s); q = std::find(++p, std::end(s), delim)) {
            v.emplace_back(p, q);
            p = q;
        }
        if (p != std::end(s))
            v.emplace_back(p, std::end(s));
    };

    //Open file
    std::ifstream file(path);
    if (!file.is_open()) {
        LENNY_LOG_WARNING("Shader file `%s` could not be successfully read!", path.c_str());
        return;
    }

    //Read file's buffer contents into streams
    std::stringstream stream;
    std::vector<std::string> token;
    for (std::string s; std::getline(file, s);) {
        token.clear();
        split(token, s, ' ');
        if (token.size() >= 2 && token[0] == "#include") {
            token[1].erase(remove(token[1].begin(), token[1].end(), '\"'), token[1].end());
            std::string includePath = LENNY_GUI_OPENGL_FOLDER + std::string("/shaders/") + token[1];
            std::ifstream sourceFile(includePath);
            s.assign((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
        }
        stream << s << std::endl;
    }

    //Close file handlers
    file.close();

    //Convert stream into string
    code = stream.str();
}

void Shader::checkShaderCompilationErrors(const unsigned int shader, const std::string &type) const {
    GLint success;
    GLchar infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        LENNY_LOG_WARNING("Shader compilation error of type %s: `%s`", type.c_str(), infoLog);
    }
}

void Shader::checkProgramCompilationErrors(const unsigned int program) const {
    GLint success;
    GLchar infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        LENNY_LOG_WARNING("Shader program linking error: `%s`", infoLog);
    }
}

}  // namespace lenny::gui