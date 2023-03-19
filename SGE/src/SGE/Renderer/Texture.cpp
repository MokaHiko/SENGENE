#include "Texture.h"
#include <glad/glad.h>

#define STBI_NO_FAILURE_STRINGS
#define STB_IMAGE_IMPLEMENTATION
#include "Renderer/ResourceManager.h"
#include <stb_image.h>

namespace SGE
{
  Texture2D::Texture2D(const char *path, TextureType type)
      : m_RendererID(0), m_Type(type)
  {
    int width, height, nChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nChannels, 0);
    if (!data)
    {
      std::cout << "TEXTURE::ERROR:: Failed to Load Image: " << path << "\n";
      stbi_image_free(data);
      return;
    }
    ProcessImageData(data, width, height, nChannels);
    stbi_image_free(data);
  }

  Texture2D::Texture2D(void *buffer, uint32_t bufferSize, TextureType type)
  {
    int width, height, nChannels;
    unsigned char *data = stbi_load_from_memory(
        (const stbi_uc *)buffer, bufferSize, &width, &height, &nChannels, 0);
    ProcessImageData(data, width, height, nChannels);
  }

  Texture2D::~Texture2D() { glDeleteTextures(1, &m_RendererID); }

  void Texture2D::Bind(uint32_t textureUnit) const
  {
    glActiveTexture(GL_TEXTURE0 + m_TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_RendererID);
  }

  void Texture2D::Unbind(uint32_t textureUnit) const
  {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  Ref<Texture2D> Texture2D::CreateTexture2D(const std::string &path)
  {
    return ResourceManager::CreateTexture(path);
  }

  Ref<Texture2D> Texture2D::CreateTexture2D(const std::string &textureName,
                                            void *buffer, uint32_t bufferSize)
  {
    return ResourceManager::CreateTexture(textureName, buffer, bufferSize);
  }

  void Texture2D::ProcessImageData(unsigned char *data, int width, int height,
                                   int nChannels)
  {
    int format = GL_RGB;
    int internalFormat = GL_RGB8;
    switch (nChannels)
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

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
  }
} // namespace SGE