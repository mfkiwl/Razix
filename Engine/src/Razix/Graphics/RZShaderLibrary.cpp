// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZShaderLibrary.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/RZSplashScreen.h"

#include "Razix/Graphics/RHI/API/RZShader.h"

#include "Razix/Utilities/RZStringUtilities.h"

namespace Razix {
    namespace Graphics {

        void RZShaderLibrary::StartUp()
        {
            // Instance is automatically created once the system is Started Up
            RAZIX_CORE_INFO("[Shader Library] Starting Up Shader Library to laod shaders");
            //Razix::RZSplashScreen::Get().setLogString("STATIC_INITIALIZATION : Starting VFS...");
            Razix::RZSplashScreen::Get().setLogString("Shader Library ...");

            // TODO: Load shader caches and partition them into RZShader which is a derivative of RZAsset. so this also a discussion for another day
            // pre-load some shaders
            //-------------------------------------------------------------------
            // Basic shaders
            loadBuiltInShader(ShaderBuiltin::Default, "//RazixContent/Shaders/Razix/Shader.Builtin.Default.rzsf");
            loadBuiltInShader(ShaderBuiltin::Skybox, "//RazixContent/Shaders/Razix/Shader.Builtin.Skybox.rzsf");
            loadBuiltInShader(ShaderBuiltin::ProceduralSkybox, "//RazixContent/Shaders/Razix/Shader.Builtin.ProceduralSkybox.rzsf");
            //-------------------------------------------------------------------
            // Renderer related shaders
            loadBuiltInShader(ShaderBuiltin::ImGui, "//RazixContent/Shaders/Razix/Shader.Builtin.ImGui.rzsf");
            //-------------------------------------------------------------------
            // Frame Graph Pass Shaders
            // Composite Pass
            loadBuiltInShader(ShaderBuiltin::Composition, "//RazixContent/Shaders/Razix/Shader.Builtin.Composition.rzsf");
            //-------------------------------------------------------------------
            // Lighting
            loadBuiltInShader(ShaderBuiltin::GBuffer, "//RazixContent/Shaders/Razix/Shader.Builtin.GBuffer.rzsf");
            //loadBuiltInShader(ShaderBuiltin::DeferredDecals, "//RazixContent/Shaders/Razix/Shader.Builtin.DeferredDecals.rzsf");
            loadBuiltInShader(ShaderBuiltin::DepthPreTest, "//RazixContent/Shaders/Razix/Shader.Builtin.DepthPreTest.rzsf");
            loadBuiltInShader(ShaderBuiltin::CSM, "//RazixContent/Shaders/Razix/Shader.Builtin.CSM.rzsf");
            loadBuiltInShader(ShaderBuiltin::PBRLighting, "//RazixContent/Shaders/Razix/Shader.Builtin.PBRLighting.rzsf");
            loadBuiltInShader(ShaderBuiltin::PBRIBL, "//RazixContent/Shaders/Razix/Shader.Builtin.PBRIBL.rzsf");
            loadBuiltInShader(ShaderBuiltin::PBRDeferredLighting, "//RazixContent/Shaders/Razix/Shader.Builtin.PBRDeferredIBL.rzsf");
            //-------------------------------------------------------------------
            // Post Processing FX
            loadBuiltInShader(ShaderBuiltin::ColorGrading, "//RazixContent/Shaders/Razix/Shader.Builtin.ColorGrading.rzsf");
            loadBuiltInShader(ShaderBuiltin::SSAO, "//RazixContent/Shaders/Razix/Shader.Builtin.SSAO.rzsf");
            loadBuiltInShader(ShaderBuiltin::GaussianBlur, "//RazixContent/Shaders/Razix/Shader.Builtin.GaussianBlur.rzsf");
            loadBuiltInShader(ShaderBuiltin::TAAResolve, "//RazixContent/Shaders/Razix/Shader.Builtin.TAAResolve.rzsf");
            loadBuiltInShader(ShaderBuiltin::Tonemap, "//RazixContent/Shaders/Razix/Shader.Builtin.Tonemap.rzsf");
            //-------------------------------------------------------------------
            // Utility Shaders (Graphics + Compute)
            // Env to CubeMap
            loadBuiltInShader(ShaderBuiltin::EnvToCubemap, "//RazixContent/Shaders/Razix/Shader.Builtin.EnvToCubeMap.rzsf");
            loadBuiltInShader(ShaderBuiltin::GenerateIrradianceMap, "//RazixContent/Shaders/Razix/Shader.Builtin.GenerateIrradianceMap.rzsf");
            loadBuiltInShader(ShaderBuiltin::GeneratePreFilteredMap, "//RazixContent/Shaders/Razix/Shader.Builtin.GeneratePreFilteredMap.rzsf");
            //------------------------------------------------------------------
            // Debug Shaders
            loadBuiltInShader(ShaderBuiltin::DebugPoint, "//RazixContent/Shaders/Razix/Shader.Builtin.DebugPoint.rzsf");
            loadBuiltInShader(ShaderBuiltin::DebugLine, "//RazixContent/Shaders/Razix/Shader.Builtin.DebugLine.rzsf");
            //-------------------------------------------------------------------
            loadBuiltInShader(ShaderBuiltin::Sprite, "//RazixContent/Shaders/Razix/Shader.Builtin.Sprite.rzsf");
            loadBuiltInShader(ShaderBuiltin::SpriteTextured, "//RazixContent/Shaders/Razix/Shader.Builtin.SpriteTextured.rzsf");
            //-------------------------------------------------------------------
            // Utility Shaders aka Math
            //-------------------------------------------------------------------
        }

        void RZShaderLibrary::ShutDown()
        {
            // FIXME: This shut down is called after Graphics is done so...do whatever the fuck is necessary
            // Destroy all the shaders
            for (auto& shader: m_BuiltinShaders)
                RZResourceManager::Get().destroyShader(shader.second);

            RAZIX_CORE_ERROR("[Shader Library] Shutting Down Shader Library");
        }

        void RZShaderLibrary::loadBuiltInShader(ShaderBuiltin shaderID, std::string shaderPath)
        {
            RZShaderHandle shader      = RZResourceManager::Get().createShaderFromFile(shaderID, shaderPath);
            m_BuiltinShaders[shaderID] = shader;

            m_BuiltinShadersReverseNameMap[Utilities::RemoveFilePathExtension(Utilities::GetFileName(shaderPath))] = shaderID;
        }

        void RZShaderLibrary::reloadShadersFromDisk()
        {
            for (auto& shader: m_BuiltinShaders) {
                auto shaderPath = RZResourceManager::Get().getShaderResource(shader.second)->getShaderFilePath();
                RZResourceManager::Get().destroyShader(shader.second);
                RZShaderHandle shaderHandle    = RZResourceManager::Get().createShaderFromFile(shader.first, shaderPath);
                m_BuiltinShaders[shader.first] = shaderHandle;
            }
        }

        RZShaderHandle RZShaderLibrary::getBuiltInShader(ShaderBuiltin builtInShaderName)
        {
            return m_BuiltinShaders[builtInShaderName];
        }

        RZShaderHandle RZShaderLibrary::getBuiltInShader(std::string shaderName)
        {
            return m_BuiltinShaders[m_BuiltinShadersReverseNameMap[shaderName]];
        }

    }    // namespace Graphics
}    // namespace Razix