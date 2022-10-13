#include "Texture.h"
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Renderer/ResourceManager.h"

namespace SGE {
	Texture2D::Texture2D(const char* path, TextureType type)
		:m_RendererID(0), m_Type(type)
	{
		int width, height, nChannels;
		unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);
		if(!data)
		{
			std::cout << "Failed to load image: " << path << "\n";
			stbi_image_free(data);
			return;
		}

		int format = GL_RGB;
		int internalFormat = GL_RGB8;
		switch(nChannels)
		{
		case 1:
			format = GL_RED;
			internalFormat = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			internalFormat = GL_RGB8;
			break;
		case 4:
			format = GL_RGBA;
			internalFormat = GL_RGBA8;
			break;
		default:
			break;
		}

		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		stbi_image_free(data);
	}

	Texture2D::~Texture2D()
	{
		glDeleteTextures(1, &m_RendererID);
	}
	
	void Texture2D::Bind(uint32_t textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + m_TextureUnit);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
	
	void Texture2D::Unbind(uint32_t textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
	
	Ref<Texture2D> Texture2D::CreateTexture2D(const std::string& path)
	{
		return ResourceManager::CreateTexture(path);
	}
}