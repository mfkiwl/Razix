#include "rzxpch.h"
#include "VKShader.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Platform/API/Vulkan/VKDevice.h"
#include "Razix/Platform/API/Vulkan/VKUtilities.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <SPIRVReflect/common/output_stream.h>
#include <glm/glm.hpp>

namespace Razix {
    namespace Graphics {

        static uint32_t GetStrideFromVulkanFormat(VkFormat format)
        {
            switch (format) {
                case VK_FORMAT_R8_SINT:
                    return sizeof(int);
                case VK_FORMAT_R32_SFLOAT:
                    return sizeof(float);
                case VK_FORMAT_R32G32_SFLOAT:
                    return sizeof(glm::vec2);
                case VK_FORMAT_R32G32B32_SFLOAT:
                    return sizeof(glm::vec3);
                case VK_FORMAT_R32G32B32A32_SFLOAT:
                    return sizeof(glm::vec4);
                case VK_FORMAT_R32G32_SINT:
                    return sizeof(glm::ivec2);
                case VK_FORMAT_R32G32B32_SINT:
                    return sizeof(glm::ivec3);
                case VK_FORMAT_R32G32B32A32_SINT:
                    return sizeof(glm::ivec4);
                case VK_FORMAT_R32G32_UINT:
                    return sizeof(glm::uvec2);
                case VK_FORMAT_R32G32B32_UINT:
                    return sizeof(glm::uvec3);
                case VK_FORMAT_R32G32B32A32_UINT:
                    return sizeof(glm::uvec4); //Need uintvec?
                default:
                    RAZIX_CORE_ERROR("Unsupported Format {0}", format);
                    return 0;
            }

            return 0;
        }

        static VkShaderStageFlags EngineToVulkanhaderStage(ShaderStage stage)
        {
            switch (stage) {
                case Razix::Graphics::ShaderStage::NONE:
                    return VK_SHADER_STAGE_ALL;
                    break;
                case Razix::Graphics::ShaderStage::VERTEX:
                    return VK_SHADER_STAGE_VERTEX_BIT;
                    break;
                case Razix::Graphics::ShaderStage::PIXEL:
                    return VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;
                case Razix::Graphics::ShaderStage::GEOMETRY:
                    return VK_SHADER_STAGE_GEOMETRY_BIT;
                    break;
                case Razix::Graphics::ShaderStage::TCS:
                    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                    break;
                case Razix::Graphics::ShaderStage::TES:
                    return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                    break;
                case Razix::Graphics::ShaderStage::COMPUTE:
                    return VK_SHADER_STAGE_COMPUTE_BIT;
                    break;
                default:
                    return VK_SHADER_STAGE_ALL_GRAPHICS;
                    break;
            }
        }

        static DescriptorType VKToEngineDescriptorType(SpvReflectDescriptorType type)
        {
            switch (type) {
                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                    return DescriptorType::IMAGE_SAMPLER;
                    break;
                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                    return DescriptorType::UNIFORM_BUFFER;
                    break;
            }
        }

        VKShader::VKShader(const std::string& filePath)
        {
            m_ShaderFilePath = filePath;

            m_Name = Razix::Utilities::GetFileName(filePath);

            // Read the *.rzsf shader file to get the necessary shader stages and it's corresponding compiled shader file virtual path
            m_ParsedRZSF = RZShader::ParseRZSF(filePath);

            // Cross compile the shaders if necessary to reflect it onto GLSL
            // TODO: Make this shit dynamic!
            CrossCompileShaders(m_ParsedRZSF, ShaderSourceType::SPIRV);

            init();
        }

        VKShader::~VKShader() { }

        void VKShader::Bind() const
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void VKShader::Unbind() const
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }

        void VKShader::CrossCompileShaders(const std::map<ShaderStage, std::string>& sources, ShaderSourceType srcType)
        {
            if (srcType != ShaderSourceType::SPIRV)
                return;
        }

        void VKShader::init()
        {
            // Reflect the SPIR-V shader to extract all the necessary information
            for (const auto& spvSource : m_ParsedRZSF) {

                std::string outPath, virtualPath;
                virtualPath = "//RazixContent/Shaders/" + spvSource.second;
                RZVirtualFileSystem::Get().resolvePhysicalPath(virtualPath, outPath);
                int64_t fileSize = RZFileSystem::GetFileSize(outPath);

                const void* spvByteCode= reinterpret_cast<void*>(RZVirtualFileSystem::Get().readFile(virtualPath));

                // Generate reflection data for a shader
                SpvReflectShaderModule module;
                SpvReflectResult result = spvReflectCreateShaderModule(fileSize, spvByteCode, &module);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV shader - ({0})!", virtualPath);

                // SPIRV-Reflect uses similar API to Vulkan, first enumerate for the count and then create the container and the use the same functions to fill the container with the enumerated data
                // Also SPIRV-Reflect API enums use same integer codes as Vulkan hence they can be type casted safely and they will work fine, this is guaranteed by the API hence we need not do extra conversions

                // Enumerate and extract shaders input variables
                uint32_t var_count = 0;
                result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect SPIRV Input variables - ({0})!", virtualPath);
                SpvReflectInterfaceVariable** input_vars = (SpvReflectInterfaceVariable**) malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
                result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);

                // Vertex Input attributes
                if (spvSource.first == ShaderStage::VERTEX) {

                    m_VertexInputStride = 0;

                    //std::cout << "---------------------------------------------" << std::endl;
                    for (size_t i = 0; i < var_count; i++) {
                        SpvReflectInterfaceVariable* inputVar = input_vars[i];

                        // Fill the vulkan input variables attribute information
                        VkVertexInputAttributeDescription verextInputattribDesc = {};
                        verextInputattribDesc.binding   = 0;
                        verextInputattribDesc.location  = inputVar->location;
                        verextInputattribDesc.format    = (VkFormat) inputVar->format;
                        verextInputattribDesc.offset    = m_VertexInputStride;

                        m_VertexInputAttributeDescriptions.push_back(verextInputattribDesc);

                        m_VertexInputStride += GetStrideFromVulkanFormat((VkFormat) inputVar->format);
                    }
                }

                // Uniform Buffers + Samplers
                // Descriptor Bindings : These bindings describe where and what kind of resources are bound to the shaders at various stages, they also store the information of the nature of resource data that is bound
                // Uniform variables usually have members and samplers none

                uint32_t descriptors_count = 0;
                result = spvReflectEnumerateDescriptorBindings(&module, &descriptors_count, nullptr);
                RAZIX_CORE_ASSERT((result == SPV_REFLECT_RESULT_SUCCESS), "Could not reflect descriptor bindings from SPIRV shader - ({0})", virtualPath);
                SpvReflectDescriptorBinding** pp_descriptor_bindings = (SpvReflectDescriptorBinding**) malloc(var_count * sizeof(SpvReflectDescriptorBinding*));
                result = spvReflectEnumerateDescriptorBindings(&module, &descriptors_count, pp_descriptor_bindings);

                //std::cout << "---------------------------------------------" << std::endl;
                for (uint32_t i = 0; i < descriptors_count; i++) {
                    SpvReflectDescriptorBinding* descriptor = pp_descriptor_bindings[i];
                    //std::cout << "SPIRV ID                  : " << descriptor->spirv_id << std::endl;
                    //std::cout << "UBO Name                  : " << descriptor->name << std::endl;
                    //std::cout << "Binding                   : " << descriptor->binding << std::endl;
                    //std::cout << "Set                       : " << descriptor->set << std::endl;
                    //std::cout << "Count                     : " << descriptor->count << std::endl;
                    //std::cout << "Descriptor Type           : " << ToStringDescriptorType(descriptor->descriptor_type) << std::endl;
                    //std::cout << "Member Count              : " << descriptor->block.member_count << std::endl;
                    //
                    //for (size_t i = 0; i < descriptor->block.member_count; i++) {
                    //
                    //    std::cout << "\t Member Name : " << descriptor->block.members[i].name << std::endl;
                    //    std::cout << "\t Member offset : " << descriptor->block.members[i].offset << std::endl;
                    //    std::cout << "\t Member size : " << descriptor->block.members[i].size << std::endl;
                    //}
                    //std::cout << "---------------------------------------------" << std::endl;

                    // First create the descriptor layout bindings, these describe where and what kind of resources are being bound to the shader per descriptor set
                    // Which means each descriptor set (i.e. for a given set ID) it stores a list of binding layouts in a map
                    VkDescriptorSetLayoutBinding setLayoutBindingInfo = {};
                    setLayoutBindingInfo.binding = descriptor->binding;
                    setLayoutBindingInfo.descriptorCount = 1; // descriptorCount is the number of descriptors contained in the binding, accessed in a shader as an array, if any (useful for Animation aka JointTransforms)
                    setLayoutBindingInfo.descriptorType = (VkDescriptorType)descriptor->descriptor_type;
                    setLayoutBindingInfo.stageFlags = EngineToVulkanhaderStage(spvSource.first);

                    m_VKSetBindingLayouts[descriptor->set].push_back(setLayoutBindingInfo);

                    // -->Also store all this data for the engine as well.
                    DescriptorLayoutBinding bindingInfo;
                    bindingInfo.binding = descriptor->binding;
                    bindingInfo.count   = 1;
                    bindingInfo.name    = descriptor->name;
                    bindingInfo.type    = VKToEngineDescriptorType(descriptor->descriptor_type);
                    bindingInfo.stage   = spvSource.first;

                    m_SetLayoutBindings[descriptor->set].push_back(bindingInfo);
                }
                // Destroy the reflection data when no longer required
                spvReflectDestroyShaderModule(&module);
            }

            // Create the Vulkan set layouts for each set ID with the descriptors (bindings) it has
            // Set layout describe how a particular descriptor and it's bindings are layed out
            for (const auto& setLayouts : m_VKSetBindingLayouts) {
                VkDescriptorSetLayoutCreateInfo layoutInfo{};
                layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.bindingCount = setLayouts.second.size();
                layoutInfo.pBindings = setLayouts.second.data();

                VkDescriptorSetLayout& setLayout = m_SetLayouts[setLayouts.first];
                if (VK_CHECK_RESULT(vkCreateDescriptorSetLayout(VKDevice::Get().getDevice(), &layoutInfo, nullptr, &setLayout)))
                    RAZIX_CORE_ERROR("[Vulkan] Failed to create descriptor set layout!");
                else RAZIX_CORE_TRACE("[Vulkan] Successfully created descriptor set layout");
            }
        }
    }
}