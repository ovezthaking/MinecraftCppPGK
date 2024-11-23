#include "ShaderProgram.h"
#include <iostream>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>


std::string ShaderProgram::s_fragmentShaderSource = R"(
#version 330 core 
out vec4 FragColor; 
in vec2 outTexCoords; 
uniform sampler2D texture1; 
void main() { 
    FragColor = texture(texture1, outTexCoords); 
} 
)";
std::string ShaderProgram::s_vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoords;

out vec2 outTexCoords;
uniform mat4 mvp;

void main() {
    gl_Position = mvp * vec4(inPosition, 1.0);
    outTexCoords = inTexCoords;
}
)";


extern GLuint CreateShader(const GLchar* shaderSource, GLenum shaderType); // Deklaracja funkcji, ¿eby mo¿na by³o jej u¿yæ
extern GLuint CreateTexture(const std::string& path); // Deklaracja funkcji, ¿eby mo¿na by³o jej u¿yæ
extern GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader, GLuint geometryShader = 0);

ShaderProgram::ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = CreateShader(s_vertexShaderSource.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = CreateShader(s_fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);
    m_programId = CreateProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


ShaderProgram::ShaderProgram()
    : m_programId(glCreateProgram()) {
    const GLuint vertexShader = CreateShader(s_vertexShaderSource.c_str(), GL_VERTEX_SHADER);


    glAttachShader(m_programId, vertexShader);

    const GLuint fragmentShader = CreateShader(s_fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);
    glAttachShader(m_programId, fragmentShader);

    glLinkProgram(m_programId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Use();
    SetInt("texture1", 0);

}

ShaderProgram::ShaderProgram(ShaderProgram&& rhs) noexcept
	: m_programId(std::exchange(rhs.m_programId, 0)) {
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& rhs) noexcept {
	if (&rhs == this) {
		return *this;
	}

	m_programId = std::exchange(rhs.m_programId, 0);

	return *this;
}




ShaderProgram::~ShaderProgram() {
    if (m_programId) {
        glDeleteProgram(m_programId);
    }
}

void ShaderProgram::Use() {
    if (m_programId) {
        glUseProgram(m_programId);
    }
    else {
        std::cerr << "Shader program ID is invalid!" << std::endl;
    }
}

void ShaderProgram::SetInt(const std::string_view name, int value) {
    GLint location = glGetUniformLocation(m_programId, name.data());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' not found or inactive!" << std::endl;
        return;
    }
    glUniform1i(location, value);
}

void ShaderProgram::SetMat4(const std::string_view name, const glm::mat4& value) {
    GLint location = glGetUniformLocation(m_programId, name.data());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' not found or inactive!" << std::endl;
        return;
    }
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}


GLint ShaderProgram::GetUniformLocation(const std::string& name) const {
    return glGetUniformLocation(m_programId, name.c_str());
}