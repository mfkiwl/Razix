#pragma once

#include "Razix/Graphics/API/RZTexture.h"

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            enum class TextureType
            {
                Texture_2D,
                Texture_3D,
                Texture_CubeMap,
                Texture_Depth,
                Texture_RenderTarget,
                Texture_SwapchainImage
            };

            class RZFrameGraphTexture
            {
            public:
                struct Desc
                {
                    // TODO: Name only exists in debug mode or use the name while creating the resource
                    TextureType                 type;
                    std::string                 name;
                    glm::vec2                   extent;
                    Graphics::RZTexture::Format format;
                };

                void create(const Desc& desc, void* allocator);
                void destroy(const Desc& desc, void* allocator);

                static std::string toString(const Desc& desc);

                Graphics::RZTexture* getHandle() { return m_Texture; }

            public:
                Graphics::RZTexture* m_Texture;
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix