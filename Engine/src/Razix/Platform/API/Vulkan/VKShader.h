#pragma once

#include "Razix/Graphics/API/RZShader.h"
#include "Razix/Graphics/API/RZVertexBufferLayout.h"

#ifdef RAZIX_RENDER_API_VULKAN

#include <vulkan/vulkan.h>

namespace Razix  { 
    namespace Graphics {
        class VKShader : public RZShader
        {
        public:
            VKShader(const std::string& filePath);
            ~VKShader();

            void Bind() const override;
            void Unbind() const override;
            void CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType) override;

            /* Gets the buffer layout information in engine internal format, this is how the shader expects the vertex buffer data to be packed while uploading to the GPU */
            inline const RZVertexBufferLayout& getBufferLayout() const { return m_BufferLayout; }
            /* Gets the vertex input variables attribute descriptions of the vertex data that are being sent to the shaders via VBO */
            inline const std::vector<VkVertexInputAttributeDescription>& getVertexAttribDescriptions() const { return m_VertexInputAttributeDescriptions; }
            /* Gets Descriptor set info that is used to create the descriptor sets */
            inline const std::vector<DescriptorSetInfo>& getDescriptorSetInfos() const { return m_DescriptorSetInfos; }
        private:
            RZVertexBufferLayout                                                    m_BufferLayout;                     /* Detailed description of the input data format of the vertex buffer that has been extracted from shader   */
            std::vector<DescriptorSetInfo>                                          m_DescriptorSetInfos;               /* Encapsulates the descriptors corresponding to a set with binding and resource information                */
            uint32_t                                                                m_VertexInputStride = 0;            /* The stride of the vertex data that is extracted from the information                                     */
            std::vector<VkVertexInputAttributeDescription>                          m_VertexInputAttributeDescriptions; /* Vulkan handle for vertex input attribute description that is used by IA/VS for understating vertex data  */
            std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>> m_VKSetBindingLayouts;              /* Vulkan handle for descriptor layout binding information per descriptor set location                      */
            std::unordered_map<uint32_t, VkDescriptorSetLayout>                     m_SetLayouts;                       /* Descriptor set layouts and their corresponding set IDs                                                   */
            std::unordered_map<ShaderStage, VkPipelineShaderStageCreateInfo>        m_ShaderCreateInfos;                /* Shader module abstractions that will be used while creating the pipeline to bind the shaders             */

        private:
            void reflectShader();
            void createShaderModules();
        };
    }
}
#endif