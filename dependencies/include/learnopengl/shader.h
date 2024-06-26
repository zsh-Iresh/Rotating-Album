#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include<string>
#include<fstream>
#include<sstream>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

class Shader
{
public:
	Shader(const char* vertex_filename, const char* fragment_filename) :m_shader_program(0)
	{
		std::string vertex_code;
		std::string fragment_code;
		std::ifstream vertex_shader_file;
		std::ifstream fragment_shader_file;

		vertex_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragment_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vertex_shader_file.open(vertex_filename);
			fragment_shader_file.open(fragment_filename);
			std::stringstream ver_ss, frg_ss;
			ver_ss << vertex_shader_file.rdbuf();
			frg_ss << fragment_shader_file.rdbuf();

			vertex_shader_file.close();
			fragment_shader_file.close();

			vertex_code = ver_ss.str();
			fragment_code = frg_ss.str();
		}
		catch (std::ifstream::failure& e) {
			std::cout << "ERROR : " << e.what() << "[" << __FILE__ << ":" << __LINE__ << "]" << std::endl;
		}

		const char* vertexShaderSource = vertex_code.c_str();
		const char* fragmentShaderSource = fragment_code.c_str();

		GLuint vertex, fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		fragment = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertex, 1, &vertexShaderSource, nullptr);
		glShaderSource(fragment, 1, &fragmentShaderSource, nullptr);

		//compile
		glCompileShader(vertex);
		checkShaderError(vertex, "COMPILE");
		glCompileShader(fragment);
		checkShaderError(fragment, "COMPILE");

		// create program
		m_shader_program = glCreateProgram();

		//attach
		glAttachShader(m_shader_program, vertex);
		glAttachShader(m_shader_program, fragment);

		//link
		glLinkProgram(m_shader_program);
		checkShaderError(m_shader_program, "LINK");

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	~Shader() {};

	void begin() { glUseProgram(m_shader_program); };
	void end() { glUseProgram(0); };

	void setInt(const std::string& name, int value) 
	{
		GLuint location = glGetUniformLocation(m_shader_program, name.c_str());
		glUniform1i(location, value);
	};
	void setFloat(const std::string& name, float value)
	{
		GLuint location = glGetUniformLocation(m_shader_program, name.c_str());
		glUniform1f(location, value);
	}
	void setMatrix4(const std::string& name, glm::mat4 value)
	{
		GLuint location = glGetUniformLocation(m_shader_program, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
	void setVec3(const std::string& name, glm::vec3& value) const
	{
		GLuint location = glGetUniformLocation(m_shader_program, name.c_str());
		glUniform3fv(location, 1, &value[0]);
	}
	void setVec3(const std::string& name, float x, float y, float z) const
	{
		GLuint location = glGetUniformLocation(m_shader_program, name.c_str());
		glUniform3f(location, x, y, z);
	}

private:
	void checkShaderError(GLuint target, std::string type)
	{
		int result = 0;
		char infoLog[1024];
		if (type == "COMPILE")
		{
			glGetShaderiv(target, GL_COMPILE_STATUS, &result);
			if (!result)
			{
				glGetShaderInfoLog(target, 1024, nullptr, infoLog);
				std::cout << "FRAGMENT : SHADER COMPILE ERROR \n" << infoLog << std::endl;
			}
		}
		else if (type == "LINK")
		{
			glGetProgramiv(target, GL_LINK_STATUS, &result);
			if (!result)
			{
				glGetProgramInfoLog(target, 1024, nullptr, infoLog);
				std::cout << "PROGRAM : SHADER LINK ERROR \n" << infoLog << std::endl;
			}

		}
		else
		{
			std::cout << "ERROR : Check Shader Error Type Is Wrong" << std::endl;
		}
	}

public:
	GLuint m_shader_program;
};