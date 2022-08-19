#pragma once

#include "Razix/Graphics/Renderers/IRZRenderer.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

namespace Razix {
    namespace Graphics {

        class RZMesh;
        class RZUniformBuffer;
        class RZVertexBuffer;
        class RZIndexBuffer;

        class RAZIX_API RZForwardRenderer : public IRZRenderer
        {
        public:
            struct ForwardLightData
            {
                alignas(16) glm::vec3 position = glm::vec3(1.0f);
                alignas(16) glm::vec3 color    = glm::vec3(1.0f);
                alignas(16) glm::vec3 viewPos  = glm::vec3(1.0f);
            };

        public:
            RZForwardRenderer() {}
            ~RZForwardRenderer() {}

            void Init() override;

            void InitDisposableResources() override;

            void Begin() override;

            void BeginScene(RZScene* scene) override;

            void Submit(RZCommandBuffer* cmdBuf) override;

            void EndScene(RZScene* scene) override;

            void End() override;

            void Present() override;

            void Resize(uint32_t width, uint32_t height) override;

            void Destroy() override;

            void OnEvent(RZEvent& event) override;

        private:
            ForwardLightData     m_ForwardLightData{};
            RZUniformBuffer*     m_ForwardLightUBO = nullptr;
            RZMaterialProperties m_TempMatProps{};
            RZUniformBuffer*     m_TempMatUBO;
            ;
        };
    }    // namespace Graphics
}    // namespace Razix
