// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/FrameGraph/Resources/RZFrameGraphTexture.h"

#include "Razix/Graphics/Lighting/RZIBL.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GlobalLightProbeData.h"

#include "Razix/Graphics/Renderers/RZDebugRenderer.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings settings, Razix::RZScene* scene)
        {
            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            m_BRDFfLUTTexture                = Graphics::RZTexture2D::CreateFromFile(RZ_DEBUG_NAME_TAG_STR_F_ARG("BRDF LUT") "//RazixContent/Textures/brdf_lut.png", "BRDF LUT");
            m_Blackboard.add<BRDFData>().lut = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("BRDF lut", {FrameGraph::TextureType::Texture_2D, "BRDF lut", {m_BRDFfLUTTexture->getWidth(), m_BRDFfLUTTexture->getHeight()}, {m_BRDFfLUTTexture->getFormat()}}, {m_BRDFfLUTTexture});

            // Load the Skybox and Global Light Probes
#if 0
            m_Skybox                     = RZIBL::convertEquirectangularToCubemap("//Textures/HDR/newport_loft.hdr");
            m_GlobalLightProbes.diffuse  = RZIBL::generateIrradianceMap(m_Skybox);
            m_GlobalLightProbes.specular = RZIBL::generatePreFilteredMap(m_Skybox);
            // Import this into the Frame Graph
            importGlobalLightProbes(m_GlobalLightProbes);
#endif

            // Cull Lights (Directional + Point) on CPU against camera Frustum First
            // TODO: Get the list of lights in the scene and cull them against the camera frustum and disable ActiveComponent for culled lights, but for now we can just ignore that
            auto                 group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
            std::vector<RZLight> sceneLights;
            for (auto& entity: group)
                sceneLights.push_back(group.get<LightComponent>(entity).light);

            // Pass the Scene AABB and Grid info for GI + Tiled lighting
            // TODO: Make this dynamic as scene glows larger
            m_SceneAABB = {glm::vec3(-76.83, -5.05, -47.31), glm::vec3(71.99, 57.17, 44.21)};
            const Maths::RZGrid sceneGrid(m_SceneAABB);

            uploadFrameData(scene, settings);

            //-------------------------------
            // Cascaded Shadow Maps x
            //-------------------------------
            m_CascadedShadowsRenderer.Init();
            m_CascadedShadowsRenderer.addPass(m_FrameGraph, m_Blackboard, scene, settings);
#if 0
            //-------------------------------
            // GI - Radiance Pass
            //-------------------------------
            m_GIPass.setGrid(sceneGrid);
            m_GIPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // GBuffer Pass
            //-------------------------------
            m_GBufferPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            // TODO: will be implemented once proper point lights support is completed
            //-------------------------------
            // [ ] Tiled Lighting Pass
            //-------------------------------

            //-------------------------------
            // [ ] SSAO Pass
            //-------------------------------

            //-------------------------------
            // [...] Deferred Lighting Pass
            //-------------------------------
            m_DeferredPass.setGrid(sceneGrid);
            m_DeferredPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);
#endif

            //-------------------------------
            // [ ] Skybox Pass
            //-------------------------------

            //-------------------------------
            // [ ] SSR Pass
            //-------------------------------

            //-------------------------------
            // [ ] Bloom Pass
            //-------------------------------

            //-------------------------------
            // [Test] Simple Shadow map Pass
            //-------------------------------

            m_ShadowRenderer.Init();
            m_ShadowRenderer.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // [Test] Forward Lighting Pass
            //-------------------------------

            auto&                frameDataBlock = m_Blackboard.get<FrameData>();
            const ShadowMapData& cascades       = m_Blackboard.get<ShadowMapData>();

            const SimpleShadowPassData shadowData = m_Blackboard.get<SimpleShadowPassData>();

            m_Blackboard.add<RTDTPassData>() = m_FrameGraph.addCallbackPass<RTDTPassData>(
                "Forward Lighting Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, RTDTPassData& data) {
                    builder.setAsStandAlonePass();

                    data.outputRT = builder.create<FrameGraph::RZFrameGraphTexture>("Scene HDR color", {FrameGraph::TextureType::Texture_RenderTarget, "Scene HDR color", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32});

                    data.depthRT = builder.create<FrameGraph::RZFrameGraphTexture>("Scene Depth", {FrameGraph::TextureType::Texture_Depth, "Scene Depth", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::DEPTH16_UNORM});

                    data.outputRT = builder.write(data.outputRT);
                    data.depthRT  = builder.write(data.depthRT);

                    builder.read(frameDataBlock.frameData);

                    builder.read(cascades.cascadedShadowMaps);

                    builder.read(shadowData.shadowMap);
                    builder.read(shadowData.lightVP);

                    m_ForwardRenderer.Init();
                },
                [=](const RTDTPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    m_ForwardRenderer.Begin(scene);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputRT).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(data.depthRT).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {rt, {true, scene->getSceneCamera().getBgColor()}}};
                    info.depthAttachment = {dt, {true, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent          = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize          = true;

                    // Set the Descriptor Set once rendering starts
                    static bool updatedSets = false;
                    if (!updatedSets) {
                        auto frameDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameDataBlock.frameData).getHandle();

                        RZDescriptor frame_descriptor{};
                        frame_descriptor.offset              = 0;
                        frame_descriptor.size                = sizeof(GPUFrameData);
                        frame_descriptor.bindingInfo.binding = 0;
                        frame_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        frame_descriptor.bindingInfo.stage   = ShaderStage::VERTEX;
                        frame_descriptor.uniformBuffer       = frameDataBuffer;

                        m_ForwardRenderer.SetFrameDataHeap(RZDescriptorSet::Create({frame_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Buffer")));

                        auto csmTextures = resources.get<FrameGraph::RZFrameGraphTexture>(shadowData.shadowMap).getHandle();

                        RZDescriptor csm_descriptor{};
                        csm_descriptor.bindingInfo.binding = 0;
                        csm_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        csm_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        csm_descriptor.texture             = csmTextures;

                        RZDescriptor shadow_data_descriptor{};
                        shadow_data_descriptor.size                = sizeof(ShadowMapData);
                        shadow_data_descriptor.bindingInfo.binding = 1;
                        shadow_data_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        shadow_data_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        shadow_data_descriptor.uniformBuffer       = resources.get<FrameGraph::RZFrameGraphBuffer>(shadowData.lightVP).getHandle();

                        m_ForwardRenderer.setCSMArrayHeap(RZDescriptorSet::Create({csm_descriptor, shadow_data_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("CSM + Matrices")));

                        updatedSets = true;
                    }

                    RHI::BeginRendering(Graphics::RHI::getCurrentCommandBuffer(), info);

                    m_ForwardRenderer.Draw(Graphics::RHI::getCurrentCommandBuffer());

                    m_ForwardRenderer.End();

                    Graphics::RHI::Submit(Graphics::RHI::getCurrentCommandBuffer());

                    Graphics::RHI::SubmitWork({}, {});
                });

            RTDTPassData forwardSceneData = m_Blackboard.get<RTDTPassData>();

            // FIXME: URGENTLY!!! Use a proper RT & DT from the forward pass
            //-------------------------------
            // Debug Scene Pass
            //-------------------------------
            m_FrameGraph.addCallbackPass(
                "Debug Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, auto& data) {
                    builder.setAsStandAlonePass();

                    RZDebugRenderer::Get()->Init();

                    builder.read(forwardSceneData.outputRT);
                    builder.read(frameDataBlock.frameData);
                },
                [=](const auto& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    RZDebugRenderer::DrawPoint(glm::vec3(0.0f), 0.1f);
                    RZDebugRenderer::DrawPoint(glm::vec3(1.0f), 0.1f);
                    RZDebugRenderer::DrawPoint(glm::vec3(2.0f), 0.1f);
                    RZDebugRenderer::DrawPoint(glm::vec3(3.0f), 0.1f);

                    RZDebugRenderer::DrawLine(glm::vec3(0.0f), glm::vec3(10.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                    RZDebugRenderer::DrawLine(glm::vec3(0.0f), glm::vec3(0.0f, 10.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    RZDebugRenderer::DrawLine(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 10.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

                    RZDebugRenderer::Get()->Begin(scene);

                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(forwardSceneData.outputRT).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {rt, {false, scene->getSceneCamera().getBgColor()}}};
                    //info.depthAttachment = {dt, {true, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize = true;

                    RHI::BeginRendering(RHI::getCurrentCommandBuffer(), info);

                    static bool updatedSets = false;
                    if (!updatedSets) {
                        auto frameDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(frameDataBlock.frameData).getHandle();

                        RZDescriptor frame_descriptor{};
                        frame_descriptor.offset              = 0;
                        frame_descriptor.size                = sizeof(GPUFrameData);
                        frame_descriptor.bindingInfo.binding = 0;
                        frame_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        frame_descriptor.bindingInfo.stage   = ShaderStage::VERTEX;
                        frame_descriptor.uniformBuffer       = frameDataBuffer;

                        RZDebugRenderer::Get()->SetFrameDataHeap(RZDescriptorSet::Create({frame_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Buffer")));
                        updatedSets = true;
                    }

                    RZDebugRenderer::Get()->Draw(RHI::getCurrentCommandBuffer());

                    RHI::EndRendering(Graphics::RHI::getCurrentCommandBuffer());

                    RZDebugRenderer::Get()->End();

                    Graphics::RHI::Submit(Graphics::RHI::getCurrentCommandBuffer());
                    Graphics::RHI::SubmitWork({}, {});
                });

            //-------------------------------
            // ImGui Pass
            //-------------------------------
            m_Blackboard.add<RTOnlyPassData>() = m_FrameGraph.addCallbackPass<RTOnlyPassData>(
                "ImGui Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, RTOnlyPassData& data) {
                    builder.setAsStandAlonePass();

                    // Upload to the Blackboard
                    //data.outputRT = builder.create<FrameGraph::RZFrameGraphTexture>("ImGui RT", {FrameGraph::TextureType::Texture_RenderTarget, "ImGui RT", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTexture::Format::RGBA32});

                    data.passDoneSemaphore = builder.create<FrameGraph::RZFrameGraphSemaphore>("ImGui Pass Signal Semaphore", {"ImGui Pass Semaphore"});

                    data.outputRT          = builder.write(forwardSceneData.outputRT);
                    data.passDoneSemaphore = builder.write(data.passDoneSemaphore);

                    builder.read(forwardSceneData.depthRT);

                    m_ImGuiRenderer.Init();
                },
                [=](const RTOnlyPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    m_ImGuiRenderer.Begin(scene);

                    struct CheckpointData
                    {
                        std::string RenderPassName = "ImGui Pass";
                    } checkpointData;

                    RHI::SetCmdCheckpoint(Graphics::RHI::getCurrentCommandBuffer(), &checkpointData);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputRT).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(forwardSceneData.depthRT).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {{rt, {false, glm::vec4(0.0f)}}};
                    info.depthAttachment  = {dt, {false, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(Graphics::RHI::getCurrentCommandBuffer(), info);

                    m_ImGuiRenderer.Draw(Graphics::RHI::getCurrentCommandBuffer());

                    m_ImGuiRenderer.End();

                    // Submit the render queue before presenting next
                    Graphics::RHI::Submit(Graphics::RHI::getCurrentCommandBuffer());

                    // Signal on a semaphore for the next pass (Final Composition pass) to wait on
                    Graphics::RHI::SubmitWork({}, {resources.get<FrameGraph::RZFrameGraphSemaphore>(data.passDoneSemaphore).getHandle()});
                });

            //-------------------------------
            // Final Image Presentation
            //-------------------------------
            m_CompositePass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            // Compile the Frame Graph
            RAZIX_CORE_INFO("Compiling FrameGraph ....");
            m_FrameGraph.compile();

            // Dump the Frame Graph for visualization
            std::string outPath;
            RZVirtualFileSystem::Get().resolvePhysicalPath("//RazixContent/FrameGraphs", outPath, true);
            RAZIX_CORE_INFO("Exporting FrameGraph .... to ({0})", outPath);
            std::ofstream os(outPath + "/forward_lighting_test.dot");
             os << m_FrameGraph;
        }

        void RZWorldRenderer::drawFrame(RZRendererSettings settings, Razix::RZScene* scene)
        {
            m_CascadedShadowsRenderer.updateCascades(scene);

            m_FrameGraph.execute(nullptr, &m_TransientResources);
        }

        void RZWorldRenderer::destroy()
        {
            // Destroy Imported Resources
            m_BRDFfLUTTexture->Release(true);

#if 0
            m_Skybox->Release(true);
            m_GlobalLightProbes.diffuse->Release(true);
            m_GlobalLightProbes.specular->Release(true);

#endif
            // Destroy Renderers
            m_CascadedShadowsRenderer.Destroy();
            m_ImGuiRenderer.Destroy();

            // Destroy Passes
            m_CompositePass.destroy();
            //m_GIPass.destroy();
            //m_GBufferPass.destroy();

            // Destroy Frame Graph Resources
            m_TransientResources.destroyResources();
        }

        void RZWorldRenderer::importGlobalLightProbes(GlobalLightProbe globalLightProbe)
        {
            auto& globalLightProbeData = m_Blackboard.add<GlobalLightProbeData>();

            globalLightProbeData.diffuseIrradianceMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Diffuse Irradiance", {FrameGraph::TextureType::Texture_CubeMap, "Diffuse Irradiance", {globalLightProbe.diffuse->getWidth(), globalLightProbe.diffuse->getHeight()}, {globalLightProbe.diffuse->getFormat()}}, {globalLightProbe.diffuse});

            globalLightProbeData.specularPreFilteredMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Specular PreFiltered", {FrameGraph::TextureType::Texture_CubeMap, "Specular PreFiltered", {globalLightProbe.specular->getWidth(), globalLightProbe.specular->getHeight()}, {globalLightProbe.specular->getFormat()}}, {globalLightProbe.specular});
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadFrameData(RZScene* scene, RZRendererSettings settings)
        {
            m_Blackboard.add<FrameData>() = m_FrameGraph.addCallbackPass<FrameData>(
                "",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, FrameData& data) {
                    builder.setAsStandAlonePass();

                    data.frameData = builder.create<FrameGraph::RZFrameGraphBuffer>("Frame Data", {"FrameData", sizeof(GPUFrameData)});

                    data.frameData = builder.write(data.frameData);
                },
                [=](const FrameData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    GPUFrameData gpuData{};
                    gpuData.time += gpuData.deltaTime;
                    gpuData.deltaTime      = RZEngine::Get().GetStatistics().DeltaTime;
                    gpuData.resolution     = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    gpuData.debugFlags     = settings.debugFlags;
                    gpuData.renderFeatures = settings.renderFeatures;

                    auto& sceneCam = scene->getSceneCamera();

                    sceneCam.setAspectRatio(f32(RZApplication::Get().getWindow()->getWidth()) / f32(RZApplication::Get().getWindow()->getHeight()));
#if 0

                    auto      lights     = scene->GetComponentsOfType<LightComponent>();
                    auto&     dir_light  = lights[0].light;
                    glm::mat4 lightView  = glm::lookAt(dir_light.getPosition(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    float     near_plane = -100.0f, far_plane = 50.0f;
                    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -far_plane * 2.0f, far_plane);
#endif

                    gpuData.camera.projection         = sceneCam.getProjection();
                    gpuData.camera.inversedProjection = glm::inverse(gpuData.camera.projection);
                    gpuData.camera.view               = sceneCam.getViewMatrix();
                    gpuData.camera.inversedView       = glm::inverse(gpuData.camera.view);
                    gpuData.camera.fov                = sceneCam.getPerspectiveVerticalFOV();
                    gpuData.camera.nearPlane          = sceneCam.getPerspectiveNearClip();
                    gpuData.camera.farPlane           = sceneCam.getPerspectiveFarClip();

                    // update and upload the UBO
                    resources.get<FrameGraph::RZFrameGraphBuffer>(data.frameData).getHandle()->SetData(sizeof(GPUFrameData), &gpuData);
                });
        }
    }    // namespace Graphics
}    // namespace Razix