// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZDescriptorSet.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#ifdef RAZIX_RENDER_API_VULKAN
    #include "Razix/Platform/API/Vulkan/VKDescriptorSet.h"
#endif

#ifdef RAZIX_RENDER_API_OPENGL
    #include "Razix/Platform/API/OpenGL/OpenGLDescriptorSet.h"
#endif

namespace Razix {
    namespace Graphics {

        RZDescriptorSet* RZDescriptorSet::Create(const std::vector<RZDescriptor>& descriptors RZ_DEBUG_NAME_TAG_E_ARG, bool layoutTransition)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            switch (Graphics::RZGraphicsContext::GetRenderAPI()) {
#ifdef RAZIX_RENDER_API_OPENGL
                case Razix::Graphics::RenderAPI::OPENGL: return new OpenGLDescriptorSet(descriptors); break;
#endif
#ifdef RAZIX_RENDER_API_VULKAN
                case Razix::Graphics::RenderAPI::VULKAN: return new VKDescriptorSet(descriptors, layoutTransition RZ_DEBUG_E_ARG_NAME); break;
#endif
#ifdef RAZIX_RENDER_API_DIRECTX12
                case Razix::Graphics::RenderAPI::D3D12:
#endif
                default: return nullptr; break;
            }
        }
    }    // namespace Graphics
}    // namespace Razix