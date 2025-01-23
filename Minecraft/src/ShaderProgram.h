#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>

class ShaderProgram {
public:
	ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);

	ShaderProgram();
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;
	ShaderProgram(ShaderProgram&& rhs) noexcept;
	GLuint GetProgramId() const;
	ShaderProgram& operator=(ShaderProgram&& rhs) noexcept;
	~ShaderProgram();

	void Use();


	void SetInt(const std::string_view name, int value);
	void SetMat4(const std::string_view name, const glm::mat4& value);

	GLint GetUniformLocation(const std::string& name) const;

	void setUniform(const std::string& name, const glm::mat4& matrix);

private:
	GLuint m_programId;

	static std::string s_vertexShaderSource;
	static std::string s_fragmentShaderSource;
};