#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once
#include "Core/Core.h"

namespace SGE {
    enum class TextureType
    {
        None = 0,
        Diffuse,
        Specular
    };
    class Texture2D
    {
    public:
        Texture2D(const char* path, TextureType type = TextureType::None);
        ~Texture2D();

        void Bind(uint32_t textureUnit = 0) const;
        void Unbind(uint32_t textureUnit = 0) const;

        static Ref<Texture2D> CreateTexture2D(const std::string& path);
        uint32_t GetID() const {return m_RendererID;}
    private:
        uint32_t m_RendererID;
        uint32_t m_TextureUnit = 0;
        TextureType m_Type;
    };
}
#endif