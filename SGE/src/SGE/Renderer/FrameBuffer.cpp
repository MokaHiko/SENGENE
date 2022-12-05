#include "Framebuffer.h"
#include <glad/glad.h>
namespace SGE {
	Framebuffer::Framebuffer(const FramebufferSpecification& spec)
		:m_Specification(spec), m_RendererID(0)
	{
		Clear();
	};

	Framebuffer::~Framebuffer()
	{
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteRenderbuffers(1, &m_DepthAttachment);

		glDeleteFramebuffers(1, &m_RendererID);
	}
	
	void Framebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}
	
	void Framebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	
	Ref<Framebuffer> Framebuffer::CreateFramebuffer(const FramebufferSpecification& spec)
	{
		return CreateRef<Framebuffer>(spec);
	}
	
	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width < 0 || height < 0) {
			return;
		}
		m_Specification.Width = width;
		m_Specification.Height = height;
		Clear();
	}
	
	uint32_t Framebuffer::GetColorAttachment() const
	{
		return m_ColorAttachment;
	}
	
	void Framebuffer::Clear()
	{
		if(m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteRenderbuffers(1, &m_DepthAttachment);
		}

		glGenFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		glGenTextures(1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Specification.Width, m_Specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glGenRenderbuffers(1, &m_DepthAttachment);
		glBindRenderbuffer(GL_RENDERBUFFER, m_DepthAttachment);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Frame Buffer is Incomplete!\n";

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}