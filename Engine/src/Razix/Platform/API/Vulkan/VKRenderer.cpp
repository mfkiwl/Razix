#include "rzxpch.h"
#include "VKRenderer.h"

#include "Razix/Core/RZEngine.h"

#include "Razix/Platform/API/Vulkan/VKContext.h"
#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKDescriptorSet.h"
#include "Razix/Platform/API/Vulkan/VKPipeline.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Razix {
    namespace Graphics {

        static constexpr uint32_t MAX_DESCRIPTOR_SET_COUNT = 1500;

        VKRenderer::VKRenderer(uint32_t width, uint32_t height)
        {
            m_RendererTitle = "Vulkan";
            m_Width = width;
            m_Height = height;

            // Create any extra descriptor pools here such as for ImGui and other needs
            std::array<VkDescriptorPoolSize, 5> pool_sizes = {
               VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
               VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
               VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
               VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
               VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 }
            };

            VkDescriptorPoolCreateInfo poolCreateInfo = {};
            poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolCreateInfo.flags = 0;
            poolCreateInfo.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
            poolCreateInfo.pPoolSizes = pool_sizes.data();
            poolCreateInfo.maxSets = MAX_DESCRIPTOR_SET_COUNT;

            // allocate the all-in-on pool
            if (VK_CHECK_RESULT(vkCreateDescriptorPool(VKDevice::Get().getDevice(), &poolCreateInfo, nullptr, &m_DescriptorPool)))
                RAZIX_CORE_ERROR("[Vulkan] Cannot allocate descriptor pool by VKRenderer!");
            else RAZIX_CORE_TRACE("[Vulkan] Successfully creates descriptor pool to allocate sets!");
        }

        VKRenderer::~VKRenderer()
        {
            //m_Context->Release();
        }

        void VKRenderer::InitAPIImpl()
        {
            // Cache the reference to the Vulkan context to avoid frequent calling
            m_Context = VKContext::Get();
        }

        void VKRenderer::BeginAPIImpl()
        {
            // Get the next image to present
            m_Context->getSwapchain()->acquireNextImage();
            // Begin recording to the command buffer
            m_Context->getSwapchain()->begin();
        }

        void VKRenderer::PresentAPIImple(RZCommandBuffer* cmdBuffer)
        {
            m_Context->getSwapchain()->end();
            m_Context->getSwapchain()->queueSubmit();
            m_Context->getSwapchain()->present();
        }

        void VKRenderer::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets)
        {
            uint32_t numDynamicDescriptorSets = 0;
            uint32_t numDesciptorSets = 0;

            for (auto descriptorSet : descriptorSets) {
                if (descriptorSet) {
                    auto vkDescSet = static_cast<VKDescriptorSet*>(descriptorSet);
                    m_DescriptorSetPool[numDesciptorSets] = vkDescSet->getDescriptorSet();
                    numDesciptorSets++;
                }
            }
            vkCmdBindDescriptorSets(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pipeline)->getPipelineLayout(), 0, numDesciptorSets, m_DescriptorSetPool, numDynamicDescriptorSets, nullptr);
        }

        void VKRenderer::BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets)
        {
            uint32_t numDynamicDescriptorSets = 0;
            uint32_t numDesciptorSets = 0;

            for (uint32_t i = 0; i < totalSets; i++) {
                auto set = descriptorSets[i];
                if (set) {
                    auto vkDescSet = static_cast<VKDescriptorSet*>(set);
                    m_DescriptorSetPool[numDesciptorSets] = vkDescSet->getDescriptorSet();
                    numDesciptorSets++;
                }
            }
            vkCmdBindDescriptorSets(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VKPipeline*>(pipeline)->getPipelineLayout(), 0, numDesciptorSets, m_DescriptorSetPool, numDynamicDescriptorSets, nullptr);
        }

        void VKRenderer::DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType /*= DataType::UNSIGNED_INT*/)
        {
            RZEngine::Get().GetStatistics().NumDrawCalls++;
            vkCmdDraw(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), count, 1, 0, 0);
        }

        void VKRenderer::DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
        {
            RZEngine::Get().GetStatistics().NumDrawCalls++;
            vkCmdDrawIndexed(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        void VKRenderer::DestroyAPIImpl()
        {
            // Destroy the descriptor pool
            vkDestroyDescriptorPool(VKDevice::Get().getDevice(), m_DescriptorPool, nullptr);
        }

        void VKRenderer::OnResizeAPIImpl(uint32_t width, uint32_t height)
        {
            vkDeviceWaitIdle(VKDevice::Get().getDevice());
            m_Context->getSwapchain().get()->OnResize(width, height);
        }

        RZSwapchain* VKRenderer::GetSwapchainImpl()
        {
            return static_cast<RZSwapchain*>(VKContext::Get()->getSwapchain().get());
        }

        void VKRenderer::BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, size_t blockSize, void* data)
        {
            //for (auto pushConstant : pushConstants) {

            //struct DefaultPushConstantData
            //{
            //    alignas(16) glm::mat4 model;
            //}modelPCData;

            ////modelPCData.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -100.0f, 0.0f));
            //modelPCData.model = tc.GetTransform();

            vkCmdPushConstants(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), static_cast<VKPipeline*>(pipeline)->getPipelineLayout(),  VK_SHADER_STAGE_VERTEX_BIT, 0, blockSize, data);
            //}
        }

        void VKRenderer::SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)
        {
            float depthBiasConstant = 1.25f;
            // Slope depth bias factor, applied depending on polygon's slope
            float depthBiasSlope = 1.75f;
            vkCmdSetDepthBias(static_cast<VKCommandBuffer*>(cmdBuffer)->getBuffer(), depthBiasConstant, 0.0f, depthBiasSlope);
        }

    }
}
