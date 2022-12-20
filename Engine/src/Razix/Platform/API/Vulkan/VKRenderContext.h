#pragma once

#include "Razix/Graphics/API/RZRenderContext.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"

/**
 * Now what is happening is when we acquire a image for the first renderer it will  
 */

namespace Razix {
    namespace Graphics {

        class VKRenderContext : public RZRenderContext
        {
        public:
            VKRenderContext(uint32_t width, uint32_t height);
            ~VKRenderContext();

            static VKRenderContext* GetVKRenderer() { return static_cast<VKRenderContext*>(s_APIInstance); }

            inline const VkDescriptorPool& getDescriptorPool() const { return m_DescriptorPool; }

        protected:
            void InitAPIImpl() override;
            void AcquireImageAPIImpl() override;
            void BeginAPIImpl(RZCommandBuffer* cmdBuffer) override;
            void SubmitImpl(RZCommandBuffer* cmdBuffer) override;
            void SubmitWorkImpl() override;
            void PresentAPIImpl() override;
            void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets) override;
            void DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType = DataType::UNSIGNED_INT) override;
            void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) override;
            void DestroyAPIImpl() override;

            void OnResizeAPIImpl(uint32_t width, uint32_t height) override;

            RZSwapchain* GetSwapchainImpl() override;

            void BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant) override;
            void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer) override;
            void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets) override;
            void SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, uint32_t width, uint32_t height) override;

            void BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo) override;
            void EndRenderingImpl(RZCommandBuffer* cmdBuffer) override;

        private:
            VKContext*       m_Context; /* Reference to the Vulkan context, we store it to avoid multiple calls */
            VkDescriptorSet  m_DescriptorSetPool[16];
            VkDescriptorPool m_DescriptorPool;
        };
    }    // namespace Graphics
}    // namespace Razix
