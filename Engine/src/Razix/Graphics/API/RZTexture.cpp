#include "rzxpch.h"
#include "RZTexture.h"
\
#include "Razix/Graphics/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_OPENGL
#include "Razix/Platform/API/OpenGL/OpenGLTexture.h"
#endif

#ifdef RAZIX_RENDER_API_VULKAN
#include "Razix/Platform/API/Vulkan/VKTexture.h"
#endif

#include "Razix/Graphics/API/RZShader.h"

namespace Razix {
    namespace Graphics {

        //-----------------------------------------------------------------------------------
        // Texture
        //-----------------------------------------------------------------------------------

        uint32_t RZTexture::calculateMipMapCount(uint32_t width, uint32_t height)
        {
            //tex: The texture mip levels are $numLevels = 1 + floor(log2(max(w, h, d)))$
            uint32_t levels = 1;
            while ((width | height) >> levels)
                levels++;

            return levels;
        }

        RZTexture::Format RZTexture::bitsToTextureFormat(uint32_t bits)
        {
            switch ((bits * 8)) {
                case 8:
                    return  RZTexture::Format::R8;
                case 16:
                    return  RZTexture::Format::RG8;
                case 24:
                    return  RZTexture::Format::RGB8;
                case 32:
                    return  RZTexture::Format::RGBA8;
                case 48:
                    return  RZTexture::Format::RGB16;
                case 64:
                    return  RZTexture::Format::RGBA16;
                default:
                    RAZIX_CORE_ASSERT(false, "[Texture] Unsupported image bit-depth! ({0})", bits);
                    return RZTexture::Format::RGB8;
            }
        }

        void RZTexture::generateDescriptorSet()
        {
            DescriptorSetInfo setInfo{};
            setInfo.setID                   = 0;
            RZDescriptor descriptor{};
            descriptor.name                 = m_Name;
            descriptor.bindingInfo.binding  = 0;
            descriptor.bindingInfo.count    = 1;
            descriptor.bindingInfo.stage    = ShaderStage::PIXEL;
            descriptor.bindingInfo.type     = DescriptorType::IMAGE_SAMPLER;
            descriptor.texture              = (RZTexture2D*)this;
            setInfo.descriptors.push_back(descriptor);

            m_DescriptorSet = Graphics::RZDescriptorSet::Create(setInfo.descriptors);
        }

        //-----------------------------------------------------------------------------------
        // Texture 2D
        //-----------------------------------------------------------------------------------

        RZTexture2D* RZTexture2D::Create(const std::string& name, uint32_t width, uint32_t height, void* data, Format format, Wrapping wrapMode, Filtering filterMode)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    return new OpenGLTexture2D(name, width, height, data, format, wrapMode, filterMode); break;
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKTexture2D(name, width, height, data, format, wrapMode, filterMode); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
            return nullptr;
        }

        RZTexture2D* RZTexture2D::CreateFromFile(const std::string& filePath, const std::string& name, Wrapping wrapMode, Filtering filterMode)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    return new OpenGLTexture2D(filePath, name, wrapMode, filterMode); break;
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKTexture2D(filePath, name, wrapMode, filterMode); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
            return nullptr;
        }

        //-----------------------------------------------------------------------------------
        // Depth Texture
        //-----------------------------------------------------------------------------------

        RZDepthTexture* RZDepthTexture::Create(uint32_t width, uint32_t height)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    break;
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKDepthTexture(width, height); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
            return nullptr;
        }

        //-----------------------------------------------------------------------------------
        // Render Texture
        //-----------------------------------------------------------------------------------

        RZRenderTexture* RZRenderTexture::Create(uint32_t width, uint32_t height, Format format, Wrapping wrapMode, Filtering filterMode)
        {
            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
                case Razix::Graphics::RenderAPI::OPENGL:    break;
                case Razix::Graphics::RenderAPI::VULKAN:    return new VKRenderTexture(width, height, format, wrapMode, filterMode); break;
                case Razix::Graphics::RenderAPI::DIRECTX11:
                case Razix::Graphics::RenderAPI::DIRECTX12:
                case Razix::Graphics::RenderAPI::GXM:
                case Razix::Graphics::RenderAPI::GCM:
                default: return nullptr;  break;
            }
            return nullptr;
        }
    }
}