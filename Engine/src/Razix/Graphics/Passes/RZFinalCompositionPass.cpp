// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFinalCompositionPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZShader.h"
#include "Razix/Graphics/RHI/API/RZSwapchain.h"

#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

namespace Razix {
    namespace Graphics {

        void RZFinalCompositionPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, RZScene* scene, RZRendererSettings& settings)
        {
            // Initialize the resources for the Pass
            init();

            RTOnlyPassData imguiPassData;
            if (settings.renderFeatures & RendererFeature_ImGui)
                imguiPassData = blackboard.get<RTOnlyPassData>();

            DescriptorSetsCreateInfos setInfos;
            Graphics::PipelineInfo    pipelineInfo{};

            // Get the final Scene Color HDR RT
            //SceneColorData sceneColor = blackboard.get<SceneColorData>();

            blackboard.add<CompositeData>() = framegraph.addCallbackPass<CompositeData>(
                "Final Composition",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, CompositeData& data) {
                    // Set this as a standalone pass (should not be culled)
                    builder.setAsStandAlonePass();

                    data.presentationTarget = builder.create<FrameGraph::RZFrameGraphTexture>("Present Image", {FrameGraph::TextureType::Texture_SwapchainImage, "Presentation Image", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::SCREEN});

                    data.depthTexture = builder.create<FrameGraph::RZFrameGraphTexture>("Depth Texture", {FrameGraph::TextureType::Texture_Depth, "Depth Texture", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::DEPTH16_UNORM});

                    data.presentationDoneSemaphore = builder.create<FrameGraph::RZFrameGraphSemaphore>("Present Semaphore", {"Composite Present Semaphore"});
                    data.imageReadySemaphore       = builder.create<FrameGraph::RZFrameGraphSemaphore>("Image Ready Semaphore", {"Composite Image Acquire Semaphore"});
                    // Writes from this pass
                    data.presentationTarget        = builder.write(data.presentationTarget);
                    data.depthTexture              = builder.write(data.depthTexture);
                    data.presentationDoneSemaphore = builder.write(data.presentationDoneSemaphore);
                    data.imageReadySemaphore       = builder.write(data.imageReadySemaphore);

                    if (settings.renderFeatures & RendererFeature_ImGui) {
                        builder.read(imguiPassData.passDoneSemaphore);
                        builder.read(imguiPassData.outputRT);
                    }

                    /**
                     * Issues:- Well pipeline creation needs a shader and some info from the Frame Graph(all the output attachments that the current frame graph pas writes to)
                     * so if in a Frame Graph pass if there are multiple renderables that uses different Materials (and diff Shaders) then we can't use the same pipeline.
                     * 
                     * Some solutions:
                     * 
                     * 1. I need to generate multiple pipelines (or PCOs) with all the possible shaders and options and choose them from a cached hash map
                     * 
                     * 2. Lazily create the pipeline during the first frame and cache them, not sure about the overhead of first frame rendering, which should be same as loading phase ig?
                     * Every time a new Material is encountered in the FrameGraph (first time only) it uses the attachment info from the FrameGraph Resources to set the R/W attachments
                     * and creates the pipeline and uses it for the rest of the application life cycle, this also helps when one needs to resize the attachments
                     * 
                     * 3. (Current Workaround) Since the pass is what specifies the shader we can think of Material as a alias for setting the properties on "a" shader, it need not be it's own
                     * It can set it for the current shader that the pass uses
                     */

                    // Build the pipeline here for this pass
                    pipelineInfo.cullMode               = Graphics::CullMode::NONE;
                    pipelineInfo.depthBiasEnabled       = false;
                    pipelineInfo.drawType               = Graphics::DrawType::TRIANGLE;
                    pipelineInfo.shader                 = Graphics::RZShaderLibrary::Get().getShader("composite_pass.rzsf");
                    pipelineInfo.transparencyEnabled    = true;
                    pipelineInfo.colorAttachmentFormats = {RZTexture::Format::BGRA8_UNORM};

                    m_Pipeline = Graphics::RZPipeline::Create(pipelineInfo RZ_DEBUG_NAME_TAG_STR_E_ARG("Composite Pass Pipeline"));

                    // Init the mesh
                    m_ScreenQuadMesh = Graphics::MeshFactory::CreatePrimitive(Razix::Graphics::MeshPrimitive::ScreenQuad);

                    // FIXME: until we use the new Descriptor Binding API which is resource faced we will do this manual linkage
                    setInfos = pipelineInfo.shader->getSetsCreateInfos();
                    for (auto& setInfo: setInfos) {
                        for (auto& descriptor: setInfo.second) {
                            descriptor.texture = Graphics::RZMaterial::GetDefaultTexture();
                        }
                        auto descSet = Graphics::RZDescriptorSet::Create(setInfo.second RZ_DEBUG_NAME_TAG_STR_E_ARG("Composite Set"));
                        m_DescriptorSets.push_back(descSet);
                    }
                },
                [=](const CompositeData& data, FrameGraph::RZFrameGraphPassResources& resources, void*) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    auto imageReadySemaphore = resources.get<FrameGraph::RZFrameGraphSemaphore>(data.imageReadySemaphore).getHandle();

                    Graphics::RHI::AcquireImage(imageReadySemaphore);

                    auto cmdBuf = m_CmdBuffers[Graphics::RHI::getSwapchain()->getCurrentImageIndex()];
                    RHI::Begin(cmdBuf);
                    RAZIX_MARK_BEGIN("Final Composition", glm::vec4(0.5f));

                    cmdBuf->UpdateViewport(RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight());

                    // Update the Descriptor Set with the new texture once
                    static bool updatedRT = false;
                    if (!updatedRT) {
                        auto& setInfos = Graphics::RZShaderLibrary::Get().getShader("composite_pass.rzsf")->getSetsCreateInfos();
                        for (auto& setInfo: setInfos) {
                            for (auto& descriptor: setInfo.second) {
                                // change the layout to be in Shader Read Only Optimal
                                descriptor.texture = resources.get<FrameGraph::RZFrameGraphTexture>(imguiPassData.outputRT).getHandle();
                            }
                            m_DescriptorSets[0]->UpdateSet(setInfo.second);
                        }
                        //updatedRT = true;
                    }

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {Graphics::RHI::getSwapchain()->GetCurrentImage(), {true, glm::vec4(0.2f)}} /*,
                        {resources.get<FrameGraph::RZFrameGraphTexture>(data.depthTexture).getHandle(), {true}}*/
                    };
                    info.extent = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize = true;

                    RHI::BeginRendering(cmdBuf, info);

                    // Bind pipeline and stuff
                    m_Pipeline->Bind(cmdBuf);

                    // Bind the descriptor sets
                    Graphics::RHI::BindDescriptorSets(m_Pipeline, cmdBuf, m_DescriptorSets);

                    // Bind the pipeline
                    m_ScreenQuadMesh->Draw(cmdBuf);

                    RHI::EndRendering(cmdBuf);

                    RAZIX_MARK_END();
                    RHI::Submit(cmdBuf);

                    // Wait on the previous pass semaphore for stuff to be done
                    auto waitOnPreviousPassSemaphore = resources.get<FrameGraph::RZFrameGraphSemaphore>(imguiPassData.passDoneSemaphore).getHandle();
                    auto presentSemaphore            = resources.get<FrameGraph::RZFrameGraphSemaphore>(data.presentationDoneSemaphore).getHandle();

                    RHI::SubmitWork({waitOnPreviousPassSemaphore, imageReadySemaphore}, {presentSemaphore});
                    RHI::Present(presentSemaphore);
                });
        }

        void RZFinalCompositionPass::init()
        {
            // Create the pipeline resource for the pass
            // Command Buffers
            for (sz i = 0; i < RAZIX_MAX_SWAP_IMAGES_COUNT; i++) {
                m_CmdBuffers[i] = RZCommandBuffer::Create();
                m_CmdBuffers[i]->Init(RZ_DEBUG_NAME_TAG_STR_S_ARG("Final Composition Pass Command Buffers"));
            }

            // Create the render pass and the attachments
        }

        void RZFinalCompositionPass::destroy()
        {
            m_Pipeline->Destroy();
            for (auto& set: m_DescriptorSets)
                set->Destroy();

            m_ScreenQuadMesh->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix