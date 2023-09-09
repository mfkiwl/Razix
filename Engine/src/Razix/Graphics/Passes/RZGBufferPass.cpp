// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZGBufferPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/FrameBlockData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZGBufferPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            // First order of business get the shader
            auto shader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::GBuffer);

            // Create the Pipeline
            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.cullMode            = Graphics::CullMode::NONE;
            pipelineInfo.shader              = shader;
            pipelineInfo.drawType            = Graphics::DrawType::TRIANGLE;
            pipelineInfo.transparencyEnabled = true;
            pipelineInfo.depthBiasEnabled    = false;
            pipelineInfo.depthTestEnabled    = true;
            // Using 32 bit f32ing point formats to support HDR colors
            pipelineInfo.colorAttachmentFormats = {
                Graphics::RZTextureProperties::Format::RGBA32F,
                Graphics::RZTextureProperties::Format::RGBA32F,
                Graphics::RZTextureProperties::Format::RGBA32F};
            pipelineInfo.depthFormat = Graphics::RZTextureProperties::Format::DEPTH32F;

            m_Pipeline = RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("GBuffer pipeline"));

            auto& frameDataBlock = blackboard.get<FrameData>();

            blackboard.add<GBufferData>() = framegraph.addCallbackPass<GBufferData>(
                "GBuffer",
                [&](GBufferData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc gbufferTexturesDesc{
                        .name   = "Normal_Metallic",
                        .width  = RZApplication::Get().getWindow()->getWidth(),
                        .height = RZApplication::Get().getWindow()->getHeight(),
                        .type   = RZTextureProperties::Type::Texture_2D,
                        .format = RZTextureProperties::Format::RGBA32F};

                    data.Normal_Metallic = builder.create<FrameGraph::RZFrameGraphTexture>("Normal_Metallic", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name = "Albedo_Roughness";

                    data.Albedo_Roughness = builder.create<FrameGraph::RZFrameGraphTexture>("Albedo_Roughness", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name = "Position_AO";

                    data.Position_AO = builder.create<FrameGraph::RZFrameGraphTexture>("Position_AO", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    gbufferTexturesDesc.name      = "Depth";
                    gbufferTexturesDesc.format    = RZTextureProperties::Format::DEPTH32F;
                    gbufferTexturesDesc.filtering = {RZTextureProperties::Filtering::FilterMode::NEAREST, RZTextureProperties::Filtering::FilterMode::NEAREST},
                    gbufferTexturesDesc.type      = RZTextureProperties::Type::Texture_Depth;

                    data.Depth = builder.create<FrameGraph::RZFrameGraphTexture>("Depth", CAST_TO_FG_TEX_DESC gbufferTexturesDesc);

                    data.Normal_Metallic  = builder.write(data.Normal_Metallic);
                    data.Albedo_Roughness = builder.write(data.Albedo_Roughness);
                    data.Position_AO      = builder.write(data.Position_AO);
                    data.Depth            = builder.write(data.Depth);

                    builder.read(frameDataBlock.frameData);
                },
                [=](const GBufferData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("GBuffer Pass", glm::vec4(1.0f, 0.6f, 0.0f, 1.0f));

                    RHI::GetCurrentCommandBuffer()->UpdateViewport(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());

                    RenderingInfo info{
                        .extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()},
                        .colorAttachments = {
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.Normal_Metallic).getHandle(), {true, glm::vec4(0.0f)}},     // location = 0
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.Albedo_Roughness).getHandle(), {true, glm::vec4(0.0f)}},    // location = 1
                            {resources.get<FrameGraph::RZFrameGraphTexture>(data.Position_AO).getHandle(), {true, glm::vec4(0.0f)}},         // location = 2
                        },
                        .depthAttachment = {resources.get<FrameGraph::RZFrameGraphTexture>(data.Depth).getHandle(), {true}},
                        .resize          = false};

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    m_Pipeline->Bind(RHI::GetCurrentCommandBuffer());

                    // Use scene to draw geometry
                    scene->drawScene(m_Pipeline, {.disableLights = true});

                    RAZIX_MARK_END();
                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                });
        }

        void RZGBufferPass::destroy()
        {
            RAZIX_UNIMPLEMENTED_METHOD
        }
    }    // namespace Graphics
}    // namespace Razix
