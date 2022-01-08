#pragma once

#include "Razix/Graphics/API/RZCommandBuffer.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

        enum class CommandBufferState : uint8_t
        {
            Idle,
            Recording,
            Ended,
            Submitted
        };

        class VKCommandBuffer : public RZCommandBuffer
        {
        public:
            VKCommandBuffer();
            ~VKCommandBuffer();

            void Init() override;
            void Init(bool primary = true, VkCommandPool cmdPool = VK_NULL_HANDLE);
            void BeginRecording() override;
            void EndRecording() override;
            void Execute() override;
            void Reset() override;
            void UpdateViewport(uint32_t width, uint32_t height) override;

            /*
            void Draw(uint32_t verticexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) override;
            void DrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;
            */
            const VkCommandBuffer& getBuffer() const { return m_CommandBuffer; }
            CommandBufferState getState() { return m_State; }

        private:
            VkCommandBuffer m_CommandBuffer;    /* Handle to the Vulkan command buffer that will be executed by the GPu     */
            VkCommandPool   m_CommandPool;      /* The command pool from which the command buffer will be allocated from    */
            CommandBufferState m_State;

        };
    }
}
#endif
