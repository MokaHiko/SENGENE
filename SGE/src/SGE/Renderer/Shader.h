#ifndef SHADER_H
#define SHADER_H

#pragma once

#include "Core/Core.h"
#include <glm/glm.hpp>

namespace SGE{
    enum class ShaderType{
        VERTEX_SHADER,
        FRAGMENT_SHADER,
        GEOMETRY_SHADER
    };

    class Shader
    {
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        ~Shader();

        static Ref<Shader> CreateShader(const std::string& vertexPath, const std::string& fragmentPath);
        static Ref<Shader> GetShader(const std::string& shaderName);

        void Bind() const;
        void Unbind() const;

    public:
        void SetBool(const std::string& name,  bool value);
        void SetInt(const std::string& name,  int value);
        void SetFloat(const std::string& name, float value);
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        void SetMat4Array(const std::string& name, const std::vector<glm::mat4>& value) const;
        void SetMat4Array(const std::string& name, const glm::mat4& value, uint32_t index = 0) const;
	    void SetVec3(const std::string& name, const glm::vec3& value) const;
    private:
        uint32_t CompileShaders(const char* shaderCode, ShaderType type);
        std::vector<char> ReadFile(const std::string& shaderPath);
        uint32_t CreateProgram(uint32_t vertexShader, uint32_t fragmentShader, uint32_t geometryShader = NULL);
    private:
        uint32_t m_RendererID = 0;
    };
}

#endif