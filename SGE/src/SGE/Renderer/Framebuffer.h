#ifndef FramebUFFER_H
#define FramebUFFER_H

#pragma once

#include "Core/Core.h"
namespace SGE {
    struct FramebufferSpecification
    {
        uint32_t Height, Width;
        uint32_t Samples = 1;
        FramebufferSpecification() : Height(0), Width(0), Samples(1) {}
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

        void Bind();
        void Unbind();

        static Ref<Framebuffer> CreateFramebuffer(const FramebufferSpecification& spec);

        void Resize(uint32_t width, uint32_t height);
        uint32_t GetColorAttachment() const;

        const FramebufferSpecification& GetFrameBufferSpecification() const { return m_Specification; }
    private:
        void Clear();

        FramebufferSpecification m_Specification;

        uint32_t m_ColorAttachment, m_DepthAttachment;
        uint32_t m_RendererID;
    };
}

#endif