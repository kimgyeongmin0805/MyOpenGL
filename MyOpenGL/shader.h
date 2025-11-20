#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
	unsigned int ID;
	
	Shader(const std::string& v_path, const std::string& f_path) {
		std::string v_str, f_str;
		std::ifstream v_fs, f_fs;
		v_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		f_fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			v_fs.open(v_path);
			f_fs.open(f_path);
			std::stringstream v_ss, f_ss;
			v_ss << v_fs.rdbuf();
			f_ss << f_fs.rdbuf();
			v_str = v_ss.str();
			f_str = f_ss.str();
		}
		catch (std::ifstream::failure& e) {
			std::cout << "ERROR::SHADER::FILE_NOT_SUCESSFULLY_READ: " << e.what() << std::endl;
		}

		const char* v_cstr = v_str.c_str();
		const char* f_cstr = f_str.c_str();

		unsigned int vertex_shader, fragment_shader;
		vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex_shader, 1, &v_cstr, nullptr);
		glCompileShader(vertex_shader);
		checkShaderError(vertex_shader, ShaderType::VertexShader);
		fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment_shader, 1, &f_cstr, nullptr);
		glCompileShader(fragment_shader);
		checkShaderError(fragment_shader, ShaderType::FragmentShader);
		ID = glCreateProgram();
		glAttachShader(ID, vertex_shader);
		glAttachShader(ID, fragment_shader);
		glLinkProgram(ID);
		checkShaderError(ID, ShaderType::Program);

		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
	}
	void use() {
		glUseProgram(ID);
	}
	void setBool(const std::string& name, bool value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF BOOL" << std::endl;
		}
		else {
			glUniform1i(location, value);
		}
	}
	void setInt(const std::string& name, int value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF INT" << std::endl;
		}
		else {
			glUniform1i(location, value);
		}
	}
	void setFloat(const std::string& name, float value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF FLOAT" << std::endl;
		}
		else {
			glUniform1f(location, value);
		}
	}
	void setVec2(const std::string& name, const glm::vec2& value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF VEC2" << std::endl;
		}
		else {
			glUniform2fv(location, 1, &value[0]);
		}
	}
	void setVec3(const std::string& name, const glm::vec3& value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF VEC3" << std::endl;
		}
		else {
			glUniform3fv(location, 1, &value[0]);
		}
	}
	void setVec4(const std::string& name, const glm::vec4& value) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF VEC4" << std::endl;
		}
		else {
			glUniform4fv(location, 1, &value[0]);
		}
	}
	void setMat2(const std::string& name, const glm::mat2& mat) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF MAT2" << std::endl;
		}
		else {
			glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
		}
	}
	void setMat3(const std::string& name, const glm::mat3& mat) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF MAT2" << std::endl;
		}
		else {
			glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
		}
	}
	void setMat4(const std::string& name, const glm::mat4& mat) const {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location == -1) {
			std::cout << "ERROR::SHADER::UNIFORM_LOCATION ERROR OF MAT2" << std::endl;
		}
		else {
			glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
		}
	}

private:
	enum class ShaderType {
		VertexShader,
		FragmentShader,
		Program,
	};
	std::string enumToString(ShaderType type) {
		switch (type) {
			case ShaderType::VertexShader:		return "VERTEX SHADER";
			case ShaderType::FragmentShader:	return "FRAGMENT SHADER";
			case ShaderType::Program:			return "PROGRAM";
		}
		return "UNKNOWN";
	}
	void checkShaderError(unsigned int shader, ShaderType type) {
		int success;
		char infoLog[1024];
		if (type != ShaderType::Program) {
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
				std::cout << "ERROR::SHADER::COMPILATION ERROR OF TYPE: " << enumToString(type) << "\n" << infoLog << std::endl;
			}
		}
		else {
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
				std::cout << "ERROR::SHADER::LINKAGE ERROR OF TYPE: " << enumToString(type) << "\n" << infoLog << std::endl;
			}
		}
	}
};

#endif  // SHADER_H
