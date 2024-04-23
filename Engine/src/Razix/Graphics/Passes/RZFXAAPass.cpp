// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFXAAPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

#include "Razix/Utilities/RZColorUtilities.h"

namespace Razix {
    namespace Graphics {

        void RZFXAAPass::addPass(FrameGraph::RZFrameGraph& framegraph, Razix::RZScene* scene, RZRendererSettings* settings)
        {
            // Create the shader and the pipeline
            auto fxaaShader = Graphics::RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::FXAA);

            RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "Pipeline.FXAA";
            pipelineInfo.shader                 = fxaaShader;
            pipelineInfo.colorAttachmentFormats = {TextureFormat::RGBA16F};
            pipelineInfo.cullMode               = Graphics::CullMode::None;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.transparencyEnabled    = false;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.depthTestEnabled       = false;

            m_Pipeline = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& sceneData = framegraph.getBlackboard().get<SceneData>();

            framegraph.addCallbackPass<FX::TAAResolveData>(
                "Pass.Builtin.Code.FXAA",
                [&](FX::TAAResolveData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    builder.read(sceneData.sceneHDR);
                    sceneData.sceneHDR = builder.write(sceneData.sceneHDR);
                },
                [=](const FX::TAAResolveData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RETURN_IF_BIT_NOT_SET(settings->renderFeatures, RendererFeature_FXAA);

                    RAZIX_TIME_STAMP_BEGIN("FXAA");
                    RAZIX_MARK_BEGIN("Pass.Builtin.Code.FXAA", Utilities::GenerateHashedColor4(46u));

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    auto accumulationRT = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();

                    RenderingInfo info{
                        .resolution       = Resolution::kWindow,
                        .colorAttachments = {{accumulationRT, {false, ClearColorPresets::OpaqueBlack}}},
                        .resize           = true};

                    RHI::BeginRendering(cmdBuffer, info);

                    // Bind pipeline and stuff
                    RHI::BindPipeline(m_Pipeline, cmdBuffer);

                    // Update descriptors on first frame
                    if (FrameGraph::RZFrameGraph::IsFirstFrame()) {
                        auto& shaderBindVars = RZResourceManager::Get().getShaderResource(fxaaShader)->getBindVars();

                        auto currentTexDescriptor = shaderBindVars["SceneTexture"];
                        if (currentTexDescriptor)
                            currentTexDescriptor->texture = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.sceneHDR).getHandle();

                        RZResourceManager::Get().getShaderResource(fxaaShader)->updateBindVarsHeaps();
                    }

                    // Push constant data for sending in the tone map mode
                    auto           res = glm::vec2(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());
                    RZPushConstant pc;
                    pc.size        = sizeof(glm::vec2);
                    pc.data        = &(res);
                    pc.shaderStage = ShaderStage::Pixel;
                    RHI::BindPushConstant(m_Pipeline, cmdBuffer, pc);

                    scene->drawScene(m_Pipeline, SceneDrawGeometryMode::ScreenQuad);

                    RHI::EndRendering(cmdBuffer);

                    RAZIX_MARK_END();
                    RAZIX_TIME_STAMP_END();
                });
        }

        void RZFXAAPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
        }
    }    // namespace Graphics
}    // namespace Razix