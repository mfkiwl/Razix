// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZMaterial.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"

#include "Razix/Graphics/Renderers/RZSystemBinding.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Utilities/RZStringUtilities.h"

#include <cereal/archives/json.hpp>

namespace Razix {
    namespace Graphics {

        RZTextureHandle RZMaterial::s_DefaultTexture;

        RZMaterial::RZMaterial(RZShader* shader)
        {
            m_Shader = shader;

            // Create the uniform buffer first
            m_MaterialPropertiesUBO = Graphics::RZUniformBuffer::Create(sizeof(MaterialProperties), &m_MaterialData.m_MaterialProperties RZ_DEBUG_NAME_TAG_STR_E_ARG("Material Properties UBO"));
        }

        void RZMaterial::Destroy()
        {
            if (m_DescriptorSet)
                m_DescriptorSet->Destroy();

            m_MaterialTextures.Destroy();
            if (m_MaterialPropertiesUBO)
                m_MaterialPropertiesUBO->Destroy();
        }

        void RZMaterial::InitDefaultTexture()
        {
            //u32  pinkTextureData    = 0xffff00ff;    // A8B8G8R8
            u32* pinkTextureDataRaw = new u32;    // A8B8G8R8
            pinkTextureDataRaw[0]   = {0xffff00ff};
            //memcpy(pinkTextureDataRaw, &pinkTextureData, sizeof(u32));
            s_DefaultTexture = RZResourceManager::Get().createTexture({.name = "Default Texture", .width = 1, .height = 1, .data = pinkTextureDataRaw, .format = RZTextureProperties::Format::RGBA8});
            //delete[] pinkTextureDataRaw;
        }

        void RZMaterial::ReleaseDefaultTexture()
        {
            RZResourceManager::Get().releaseTexture(s_DefaultTexture);
        }

        void RZMaterial::loadFromFile(const std::string& path)
        {
            m_Name = Utilities::RemoveFilePathExtension(Utilities::GetFileName(path));
            // Using the Name of the Material search the //Assets/Materials + MeshName_MaterialName.rzmaterial file and load it and set the material Data
            Razix::Graphics::MaterialData matData{};
            std::string                   matPhysicalPath;
            if (RZVirtualFileSystem::Get().resolvePhysicalPath(path, matPhysicalPath)) {
                if (!matPhysicalPath.empty()) {
                    std::ifstream AppStream;
                    AppStream.open(matPhysicalPath, std::ifstream::in);
                    cereal::JSONInputArchive inputArchive(AppStream);

                    inputArchive(cereal::make_nvp("$MATERIAL", matData));
                }
            }
            setProperties(matData.m_MaterialProperties);
            loadMaterialTexturesFromFiles(matData.m_MaterialTexturePaths);
            createDescriptorSet();
        }

        void RZMaterial::saveToFile(const std::string& path)
        {
            if (path.empty()) {
                // Just resolve the folder path to save to
                std::string matPath = "//Assets/Materials/";
                std::string matPhysicalPath;
                if (RZVirtualFileSystem::Get().resolvePhysicalPath(matPath, matPhysicalPath, true)) {
                    matPhysicalPath += std::string(m_Name) + ".rzmaterial";
                    std::ofstream             opAppStream(matPhysicalPath);
                    cereal::JSONOutputArchive defArchive(opAppStream);
                    defArchive(cereal::make_nvp("$MATERIAL", m_MaterialData));
                }
            } else {
                std::string matPhysicalPath;
                if (RZVirtualFileSystem::Get().resolvePhysicalPath(path, matPhysicalPath)) {
                    std::ofstream             opAppStream(matPhysicalPath);
                    cereal::JSONOutputArchive defArchive(opAppStream);
                    defArchive(cereal::make_nvp("$MATERIAL", m_MaterialData));
                }
            }
        }

        void RZMaterial::loadMaterialTexturesFromFiles(MaterialTexturePaths paths)
        {
            m_MaterialData.m_MaterialTexturePaths = paths;

            if (paths.albedo && paths.albedo != "") {
                auto fileName = Razix::Utilities::GetFileName(paths.albedo);
                if (!fileName.empty())
                    m_MaterialTextures.albedo = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.albedo);
            }

            if (paths.ao && paths.ao != "") {
                auto fileName = Razix::Utilities::GetFileName(paths.ao);
                if (!fileName.empty())
                    m_MaterialTextures.ao = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.ao);
            }

            if (paths.emissive && paths.emissive != "") {
                auto fileName = Razix::Utilities::GetFileName(paths.emissive);
                if (!fileName.empty())
                    m_MaterialTextures.emissive = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.emissive);
            }

            if (paths.metallic && paths.metallic != "") {
                auto fileName = Razix::Utilities::GetFileName(paths.metallic);
                if (!fileName.empty())
                    m_MaterialTextures.metallic = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.metallic);
            }

            if (paths.normal && paths.normal != "") {
                auto fileName = Razix::Utilities::GetFileName(paths.normal);
                if (!fileName.empty())
                    m_MaterialTextures.normal = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.normal);
            }

            if (paths.roughness && paths.roughness != "") {
                auto fileName = Razix::Utilities::GetFileName(paths.roughness);
                if (!fileName.empty())
                    m_MaterialTextures.roughness = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.roughness);
            }

            if (paths.specular && paths.specular != "") {
                auto fileName = Razix::Utilities::GetFileName(paths.specular);
                if (!fileName.empty())
                    m_MaterialTextures.specular = RZResourceManager::Get().createTextureFromFile({.name = fileName}, paths.specular);
            }

            setTexturesUpdated(true);
        }

        void RZMaterial::createDescriptorSet()
        {
            // Create the descriptor set for material properties data and it's textures
            // How about renderer data for forward lights info + system vars???? How to associate and update?
            auto setInfos = m_Shader->getSetsCreateInfos();
            for (auto& setInfo: setInfos) {
                if (setInfo.first == BindingTable_System::BINDING_SET_SYSTEM_MAT_PROPS) {
                    for (auto& descriptor: setInfo.second) {
                        // Find the material properties UBO and assign it's UBO to this slot
                        if (descriptor.bindingInfo.type == Graphics::DescriptorType::UNIFORM_BUFFER) {
                            descriptor.uniformBuffer = m_MaterialPropertiesUBO;
                        } else if (descriptor.bindingInfo.type == Graphics::DescriptorType::IMAGE_SAMPLER) {
                            // Choose the mat textures based on the workflow & preset
                            switch (descriptor.bindingInfo.binding) {
                                case TextureBindingTable::TEX_BINDING_IDX_ALBEDO:
                                    descriptor.texture = m_MaterialTextures.albedo.isValid() ? m_MaterialTextures.albedo : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingAlbedoMap = true;
                                    break;
                                case TextureBindingTable::TEX_BINDING_IDX_NORMAL:
                                    descriptor.texture = m_MaterialTextures.normal.isValid() ? m_MaterialTextures.normal : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingNormalMap = true;
                                    break;
                                case TextureBindingTable::TEX_BINDING_IDX_METALLLIC:
                                    descriptor.texture = m_MaterialTextures.metallic.isValid() ? m_MaterialTextures.metallic : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingMetallicMap = true;
                                    break;
                                case TextureBindingTable::TEX_BINDING_IDX_ROUGHNESS:
                                    descriptor.texture = m_MaterialTextures.roughness.isValid() ? m_MaterialTextures.roughness : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingRoughnessMap = true;
                                    break;
                                case TextureBindingTable::TEX_BINDING_IDX_SPECULAR:
                                    descriptor.texture = m_MaterialTextures.specular.isValid() ? m_MaterialTextures.specular : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingSpecular = true;
                                    break;
                                case TextureBindingTable::TEX_BINDING_IDX_EMISSIVE:
                                    descriptor.texture = m_MaterialTextures.emissive.isValid() ? m_MaterialTextures.emissive : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingEmissiveMap = true;
                                    break;
                                case TextureBindingTable::TEX_BINDING_IDX_AO:
                                    descriptor.texture = m_MaterialTextures.ao.isValid() ? m_MaterialTextures.ao : s_DefaultTexture;
                                    if (descriptor.texture != s_DefaultTexture)
                                        m_MaterialData.m_MaterialProperties.isUsingAOMap = true;
                                    break;
                                default:
                                    descriptor.texture = s_DefaultTexture;
                                    break;
                            }
                        }
                    }
                    m_DescriptorSet = RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("BINDING_SET_USER_MAT_SAMPLERS"));
                }
                // This holds the descriptor sets for the material Properties and Samplers
                // Now each mesh will have a material instance so each have their own sets so not a problem
                // TODO: Make sure the material instances similar to unreal exist with different Desc Sets for mat props both with same shader instance, Simple Solution: Use a shader library to load the same shader, of course we give the shader so that's possible
            }
        }

        void RZMaterial::setTextures(MaterialTextures& textures)
        {
            m_MaterialTextures.albedo    = textures.albedo;
            m_MaterialTextures.normal    = textures.normal;
            m_MaterialTextures.metallic  = textures.metallic;
            m_MaterialTextures.roughness = textures.roughness;
            m_MaterialTextures.specular  = textures.specular;
            m_MaterialTextures.emissive  = textures.emissive;
            m_MaterialTextures.ao        = textures.ao;

            setTexturesUpdated(true);
        }

        void RZMaterial::setProperties(MaterialProperties& props)
        {
            memcpy(&m_MaterialData.m_MaterialProperties, &props, sizeof(MaterialProperties));
            m_MaterialPropertiesUBO->SetData(sizeof(MaterialProperties), &m_MaterialData.m_MaterialProperties);
        }

        void RZMaterial::setTexturePaths(MaterialTexturePaths& paths)
        {
            memcpy(&m_MaterialData.m_MaterialTexturePaths, &paths, sizeof(paths));
        }

        void RZMaterial::Bind()
        {
            //  Check if the descriptor sets need to be built or updated and do that by deleting it and creating a new one
            if (!m_DescriptorSet || getTexturesUpdated()) {
                if (m_DescriptorSet)
                    m_DescriptorSet->Destroy();

                createDescriptorSet();
                setTexturesUpdated(false);
                // Since the mat props have been updated regarding isTextureAvailable or not we need to update the UBO data again
                setProperties(m_MaterialData.m_MaterialProperties);
            }
            // Since we need to bind all the sets at once IDK about using bind, how does the mat get the Render System Descriptors to bind???
            // This possible if do something like Unity does, have a Renderer Component for every renderable entity in the scene ==> this makes
            // it easy for get info about Culling too, using this we can easily get the System Sets and Bind them
            // For now since we use the same shader we can just let the renderer Bind it and the material will give the Renderer necessary Sets to bind
        }

        void MaterialTextures::Destroy()
        {
            if (albedo != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().releaseTexture(albedo);
            if (normal != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().releaseTexture(normal);
            if (metallic != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().releaseTexture(metallic);
            if (roughness != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().releaseTexture(roughness);
            if (specular != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().releaseTexture(specular);
            if (emissive != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().releaseTexture(emissive);
            if (ao != RZMaterial::GetDefaultTexture())
                RZResourceManager::Get().releaseTexture(ao);
        }

        RZMaterial* GetDefaultMaterial()
        {
            if (DefaultMaterial == nullptr) {
                auto shader     = Graphics::RZShaderLibrary::Get().getShader("forward_renderer.rzsf");
                DefaultMaterial = new RZMaterial(shader);
                DefaultMaterial->setName("DefaultMaterial");
                Razix::Graphics::MaterialData matData{};
                DefaultMaterial->setProperties(matData.m_MaterialProperties);
                DefaultMaterial->loadMaterialTexturesFromFiles(matData.m_MaterialTexturePaths);
                DefaultMaterial->createDescriptorSet();
                return DefaultMaterial;
            } else
                return DefaultMaterial;
        }
    }    // namespace Graphics
}    // namespace Razix