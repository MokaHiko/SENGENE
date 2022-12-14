#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/ResourceManager.h"

namespace SGE{
	Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		auto vertCode = ReadFile(vertexPath);
		auto fragCode = ReadFile(fragmentPath);

		uint32_t vertexShader = CompileShaders(vertCode.data(), ShaderType::VERTEX_SHADER);
		uint32_t fragmentShader = CompileShaders(fragCode.data(), ShaderType::FRAGMENT_SHADER);

		m_RendererID = CreateProgram(vertexShader, fragmentShader);

		printf("Shader::Vertex %s ==> SUCCESS\n", vertexPath.c_str());
		printf("Shader::Fragment %s ==> SSUCCESS\n",fragmentPath.c_str());

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}
	
	Ref<Shader> Shader::CreateShader(const std::string& vertexPath, const std::string& fragmentPath)
	{
		return ResourceManager::CreateShader(vertexPath, fragmentPath);
	}
	
	Ref<Shader> Shader::GetShader(const std::string& shaderName)
	{
		return ResourceManager::GetShader(shaderName);
	}
	
	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	
	void Shader::Unbind() const
	{
		glUseProgram(0);
	}
	
	void Shader::SetBool(const std::string& name,  bool value)
	{
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}
	
	void Shader::SetInt(const std::string& name,  int value)
	{
		glUniform1i(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}
	
	void Shader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(glGetUniformLocation(m_RendererID, name.c_str()), value);
	}
	
	void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::SetMat4Array(const std::string& name, const std::vector<glm::mat4>& value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, name.c_str()), value.size(), GL_FALSE, glm::value_ptr(value[0]));
	}

	void Shader::SetMat4Array(const std::string& name, const glm::mat4& value, uint32_t index) const
	{
		std::stringstream ss;
		ss << name << "[" << index << "]";
		glUniformMatrix4fv(glGetUniformLocation(m_RendererID, ss.str().c_str() ), 1, GL_FALSE, glm::value_ptr(value[0]));
	}

	void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3f(glGetUniformLocation(m_RendererID, name.c_str()), value.x, value.y, value.z);
	}

	uint32_t Shader::CompileShaders(const char* shaderCode, ShaderType type)
	{
		uint32_t shader = NULL;
		switch(type)
		{
			case ShaderType::VERTEX_SHADER:
				shader = glCreateShader(GL_VERTEX_SHADER);
				break;
			case ShaderType::FRAGMENT_SHADER:
				shader = glCreateShader(GL_FRAGMENT_SHADER);
				break;
			default:
				break;
		}

		int success;
		char infoLog[512];
		glShaderSource(shader, 1, &shaderCode, nullptr);
		glCompileShader(shader);

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(shader, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER:::COMPILATION: " << infoLog << std::endl;
			throw std::runtime_error("Failed to compile shader!");
		}

		return shader;
	}
	
	std::vector<char> Shader::ReadFile(const std::string& shaderPath)
	{
		std::ifstream file;

		// vertex shader
		uint32_t shader = -1;
		file.open(shaderPath, std::ios::ate | std::ios::binary);

		if(!file.is_open())
			throw std::runtime_error("Failed to open file at " + shaderPath);

		size_t size = (size_t)file.tellg();

		std::vector<char> buffer(size + 1);
		buffer.push_back('\0');
		file.seekg(0);

		file.read(buffer.data(), size);
		file.close();

		return buffer;
	}
	
	uint32_t Shader::CreateProgram(uint32_t vertexShader, uint32_t fragmentShader, uint32_t geometryShader)
	{
		uint32_t shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		int success;
		char infoLog[512];
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

		if(!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
			std::cout << "ERROR::SHADER:::PROGRAM: " << infoLog << std::endl;
			throw std::runtime_error("Failed to link shader program");
		}

		return shaderProgram;
	}
}