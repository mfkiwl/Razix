
// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZWorldRenderer.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZEngine.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"

#include "Razix/Graphics/FrameGraph/RZBlackboard.h"
#include "Razix/Graphics/FrameGraph/RZFrameGraph.h"
#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"
#include "Razix/Graphics/Resources/RZFrameGraphSemaphore.h"
#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Graphics/Lighting/RZImageBasedLightingProbesManager.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/FrameBlockData.h"

#include "Razix/Graphics/Renderers/RZDebugRenderer.h"

#include "Razix/Scene/Components/RZComponents.h"
#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZWorldRenderer::buildFrameGraph(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            // Upload buffers/textures Data to the FrameGraph and GPU initially
            // Upload BRDF look up texture to the GPU
            m_BRDFfLUTTextureHandle = RZResourceManager::Get().createTextureFromFile({.name = "BRDF LUT", .enableMips = false}, "//RazixContent/Textures/brdf_lut.png");

            m_NoiseTextureHandle = RZResourceManager::Get().createTextureFromFile({.name = "Noise Texture", .wrapping = RZTextureProperties::Wrapping::REPEAT, .enableMips = false}, "//RazixContent/Textures/volumetric_clouds_noise.png");

            const auto& BRDFfLUTTextureDesc  = RZResourceManager::Get().getPool<RZTexture>().get(m_BRDFfLUTTextureHandle)->getDescription();
            m_Blackboard.add<BRDFData>().lut = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("BRDF lut", CAST_TO_FG_TEX_DESC BRDFfLUTTextureDesc, {m_BRDFfLUTTextureHandle});

            const auto& NoiseTextureDesc                          = RZResourceManager::Get().getPool<RZTexture>().get(m_NoiseTextureHandle)->getDescription();
            m_Blackboard.add<VolumetricCloudsData>().noiseTexture = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Noise Texture", CAST_TO_FG_TEX_DESC NoiseTextureDesc, {m_NoiseTextureHandle});

            // Load the Skybox and Global Light Probes
            // FIXME: This is hard coded make this a user land material
            m_GlobalLightProbes.skybox   = RZImageBasedLightingProbesManager::convertEquirectangularToCubemap("//Assets/Textures/HDR/newport_loft.hdr");
            m_GlobalLightProbes.diffuse  = RZImageBasedLightingProbesManager::generateIrradianceMap(m_GlobalLightProbes.skybox);
            m_GlobalLightProbes.specular = RZImageBasedLightingProbesManager::generatePreFilteredMap(m_GlobalLightProbes.skybox);
            // Import this into the Frame Graph
            importGlobalLightProbes(m_GlobalLightProbes);

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
            uploadLightsData(scene, settings);

#if 0
            //-------------------------------
            // Cascaded Shadow Maps x
            //-------------------------------
            m_CascadedShadowsRenderer.Init();
            m_CascadedShadowsRenderer.addPass(m_FrameGraph, m_Blackboard, scene, settings);
            //-------------------------------
            // GI - Radiance Pass
            //-------------------------------
            m_GIPass.setGrid(sceneGrid);
            m_GIPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

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
            // [ ] SSR Pass
            //-------------------------------

            //-------------------------------
            // GBuffer Pass
            //-------------------------------
            m_GBufferPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // [Test] Simple Shadow map Pass
            //-------------------------------
            m_ShadowPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

#if 0
            //-------------------------------
            // [Test] Omni-Dir Shadow Pass
            //-------------------------------

            m_Blackboard.add<OmniDirectionalShadowPassData>() = m_FrameGraph.addCallbackPass<OmniDirectionalShadowPassData>(
                "Omni-Directional shadow pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, OmniDirectionalShadowPassData& data) {
                },
                [=](const OmniDirectionalShadowPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                });
#endif

            //-------------------------------
            // [Test] Forward Lighting Pass
            //-------------------------------
            auto& frameDataBlock = m_Blackboard.get<FrameData>();
            auto& shadowData     = m_Blackboard.get<SimpleShadowPassData>();

#if 0
            m_Blackboard.add<SceneData>() = m_FrameGraph.addCallbackPass<SceneData>(
                "Forward Lighting Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SceneData& data) {
                    builder.setAsStandAlonePass();

                    data.outputHDR = builder.create<FrameGraph::RZFrameGraphTexture>("Scene HDR color", {FrameGraph::RZTextureProperties::Type::Texture_2D, "Scene HDR color", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTextureProperties::Format::RGBA32F});

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>("Scene Depth", {FrameGraph::RZTextureProperties::Type::Texture_Depth, "Scene Depth", {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()}, RZTextureProperties::Format::DEPTH16_UNORM});

                    data.outputHDR = builder.write(data.outputHDR);
                    data.depth     = builder.write(data.depth);

                    builder.read(frameDataBlock.frameData);

                    //builder.read(shadowData.shadowMap);
                    //builder.read(shadowData.lightVP);

                    m_ForwardRenderer.Init();
                },
                [=](const SceneData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    m_ForwardRenderer.Begin(scene);

                    auto rtHDR = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle();
                    auto dt    = resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle();

                    RenderingInfo info{};
                    info.colorAttachments = {
                        {rtHDR, {true, scene->getSceneCamera().getBgColor()}},
                    };
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

                        m_ForwardRenderer.SetFrameDataHeap(RZDescriptorSet::Create({frame_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Buffer Forward")));

    #if 0
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
    #endif

                        m_ForwardRenderer.setCSMArrayHeap(RZDescriptorSet::Create({ /*csm_descriptor, shadow_data_descriptor*/ } RZ_DEBUG_NAME_TAG_STR_E_ARG("CSM + Matrices")));

                        updatedSets = true;
                    }

                    RHI::BeginRendering(Graphics::RHI::GetCurrentCommandBuffer(), info);

                    m_ForwardRenderer.Draw(Graphics::RHI::GetCurrentCommandBuffer());

                    m_ForwardRenderer.End();

                    Graphics::RHI::Submit(Graphics::RHI::GetCurrentCommandBuffer());

                    Graphics::RHI::SubmitWork({}, {});
                });
#endif
            //-------------------------------
            // PBR Pass
            //-------------------------------
            m_PBRLightingPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);
            SceneData sceneData = m_Blackboard.get<SceneData>();

            //-------------------------------
            // [x] Skybox Pass
            //-------------------------------
            m_SkyboxPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // [x] Bloom Pass
            //-------------------------------
            //if (settings.renderFeatures & RendererFeature_Bloom)
            //    m_BloomPass.addPass(m_FrameGraph, m_Blackboard, scene, settings);

            //-------------------------------
            // Debug Scene Pass
            //-------------------------------
#if 1
            m_FrameGraph.addCallbackPass(
                "Debug Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, auto& data) {
                    builder.setAsStandAlonePass();

                    RZDebugRenderer::Get()->Init();

                    builder.read(sceneData.outputHDR);
                    builder.read(sceneData.depth);
                    builder.read(frameDataBlock.frameData);
                },
                [=](const auto& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    // Origin point
                    RZDebugRenderer::DrawPoint(glm::vec3(0.0f), 0.1f);

                    // X, Y, Z lines
                    RZDebugRenderer::DrawLine(glm::vec3(-100.0f, 0.0f, 0.0f), glm::vec3(100.0f, 0.0f, 0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
                    RZDebugRenderer::DrawLine(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(0.0f, 100.0f, 0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    RZDebugRenderer::DrawLine(glm::vec3(0.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 100.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));

                    // Grid
                    RZDebugRenderer::DrawGrid(25, glm::vec4(0.75f));

                    // Draw all lights in the scene
                    auto lights = scene->GetComponentsOfType<LightComponent>();
                    for (auto& lightComponent: lights) {
                        if (lightComponent.light.getType() == LightType::Point)
                            RZDebugRenderer::DrawLight(&lights[0].light, glm::vec4(0.8f, 0.65f, 0.0f, 1.0f));
                    }

                    // Draw AABBs for all the Meshes in the Scene

                    auto mesh_group = scene->getRegistry().group<MeshRendererComponent>(entt::get<TransformComponent>);
                    for (auto entity: mesh_group) {
                        // Draw the mesh renderer components
                        const auto& [mrc, mesh_trans] = mesh_group.get<MeshRendererComponent, TransformComponent>(entity);

                        // Bind push constants, VBO, IBO and draw
                        glm::mat4 transform = mesh_trans.GetGlobalTransform();

                        RZDebugRenderer::DrawAABB(mrc.Mesh->getBoundingBox().transform(transform), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
                    }

                    RZDebugRenderer::Get()->Begin(scene);

                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_CORE);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.outputHDR).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.depth).getHandle();

                    RenderingInfo info{
                        .extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()},
                        .colorAttachments = {{rt, {false, scene->getSceneCamera().getBgColor()}}},
                        .depthAttachment  = {dt, {false, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}},
                        .resize           = true};

                    auto cmdBuffer = RHI::GetCurrentCommandBuffer();

                    RHI::BeginRendering(cmdBuffer, info);

                    RZDebugRenderer::Get()->Draw(cmdBuffer);

                    RHI::EndRendering(cmdBuffer);

                    RZDebugRenderer::Get()->End();
                });
#endif

            //-------------------------------
            // ImGui Pass
            //-------------------------------
            m_Blackboard.add<RTOnlyPassData>() = m_FrameGraph.addCallbackPass<RTOnlyPassData>(
                "ImGui Pass",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, RTOnlyPassData& data) {
                    builder.setAsStandAlonePass();

                    data.passDoneSemaphore = builder.create<FrameGraph::RZFrameGraphSemaphore>("ImGui Pass Signal Semaphore", {"ImGui Pass Semaphore"});

                    data.outputRT          = builder.write(sceneData.outputHDR);
                    data.passDoneSemaphore = builder.write(data.passDoneSemaphore);

                    builder.read(sceneData.depth);

                    m_ImGuiRenderer.Init();
                },
                [=](const RTOnlyPassData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
#if 0
                    m_ImGuiRenderer.Begin(scene);

                    auto rt = resources.get<FrameGraph::RZFrameGraphTexture>(data.outputRT).getHandle();
                    auto dt = resources.get<FrameGraph::RZFrameGraphTexture>(sceneData.depth).getHandle();

                    RenderingInfo info{
                        .extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()},
                        .colorAttachments = {{rt, {false, glm::vec4(0.0f)}}},
                        .depthAttachment  = {dt, {false, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)}},
                        .resize           = true};

                    RHI::BeginRendering(Graphics::RHI::GetCurrentCommandBuffer(), info);

                    m_ImGuiRenderer.Draw(Graphics::RHI::GetCurrentCommandBuffer());

                    m_ImGuiRenderer.End();
#endif
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
            std::ofstream os(outPath + "/pbr_lighting_test.dot");
            os << m_FrameGraph;
        }

        void RZWorldRenderer::drawFrame(RZRendererSettings& settings, Razix::RZScene* scene)
        {
            RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

            // Update calls passes
            m_CascadedShadowsRenderer.updateCascades(scene);
            m_SkyboxPass.useProceduralSkybox(settings.useProceduralSkybox);

            // Main Frame Graph World Rendering Loop
            {
                // Acquire Image to render onto
                Graphics::RHI::AcquireImage(nullptr);

                // Begin Recording  onto the command buffer, select one as per the frame idx
                RHI::Begin(Graphics::RHI::GetCurrentCommandBuffer());

                // Begin Frame Marker
                RAZIX_MARK_BEGIN("Frame [back buffer #" + std::to_string(RHI::GetSwapchain()->getCurrentImageIndex()), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

                // Execute the Frame Graph passes
                m_FrameGraph.execute(nullptr, &m_TransientResources);

                // End Frame Marker
                RAZIX_MARK_END();

                // Submit the render queue before presenting next
                Graphics::RHI::Submit(Graphics::RHI::GetCurrentCommandBuffer());

                // Signal on a semaphore for the presentation engine to wait on
                //Graphics::RHI::SubmitWork();

                // Present the image to presentation engine as soon as rendering to COLOR_ATTACHMENT is done
                Graphics::RHI::Present(nullptr);
            }
        }

        void RZWorldRenderer::destroy()
        {
            // Destroy Imported Resources
            //m_BRDFfLUTTextureHandle->Release(true);
            //m_NoiseTextureHandle->Release(true);

#if 1
            //m_GlobalLightProbes.skybox->Release(true);
            //m_GlobalLightProbes.diffuse->Release(true);
            //m_GlobalLightProbes.specular->Release(true);

#endif
            // Destroy Renderers
            //m_ForwardRenderer.Destroy();
            //m_CascadedShadowsRenderer.Destroy();
            m_ImGuiRenderer.Destroy();
            RZDebugRenderer::Get()->Destroy();

            // Destroy Passes
            m_PBRLightingPass.destroy();
            m_SkyboxPass.destroy();
            m_CompositePass.destroy();
            m_ShadowPass.destroy();
            //m_GIPass.destroy();
            //m_GBufferPass.destroy();

            // Destroy Frame Graph Resources
            m_TransientResources.destroyResources();
        }

        void RZWorldRenderer::importGlobalLightProbes(LightProbe globalLightProbe)
        {
            auto& globalLightProbeData = m_Blackboard.add<GlobalLightProbeData>();

            const auto& SkyboxDesc   = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.skybox)->getDescription();
            const auto& DiffuseDesc  = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.diffuse)->getDescription();
            const auto& SpecularDesc = RZResourceManager::Get().getPool<RZTexture>().get(globalLightProbe.specular)->getDescription();

            globalLightProbeData.environmentMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Environment Map", {.name = "Environment Map", .width = SkyboxDesc.width, .height = SkyboxDesc.height, .type = RZTextureProperties::Type::Texture_CubeMap, .format = SkyboxDesc.format}, {globalLightProbe.skybox});

            globalLightProbeData.diffuseIrradianceMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Diffuse Irradiance", {.name = "Diffuse Irradiance", .width = DiffuseDesc.width, .height = DiffuseDesc.height, .type = RZTextureProperties::Type::Texture_CubeMap, .format = DiffuseDesc.format}, {globalLightProbe.diffuse});

            globalLightProbeData.specularPreFilteredMap = m_FrameGraph.import <FrameGraph::RZFrameGraphTexture>("Specular PreFiltered", {.name = "Specular PreFiltered", .width = SpecularDesc.width, .height = SpecularDesc.height, .type = RZTextureProperties::Type::Texture_CubeMap, .format = SpecularDesc.format}, {globalLightProbe.specular});
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadFrameData(RZScene* scene, RZRendererSettings& settings)
        {
            m_Blackboard.add<FrameData>() = m_FrameGraph.addCallbackPass<FrameData>(
                "Frame Data Upload",
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
                    // Test code to view from the Directional Light POV to configure shadow map
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
                    auto frameDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.frameData).getHandle();
                    frameDataBuffer->SetData(sizeof(GPUFrameData), &gpuData);

                    if (!Graphics::RHI::Get().getFrameDataSet()) {
                        RZDescriptor descriptor{};
                        descriptor.bindingInfo.binding = 0;
                        descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        descriptor.bindingInfo.stage   = ShaderStage::VERTEX;
                        descriptor.uniformBuffer       = frameDataBuffer;
                        auto m_FrameDataSet            = RZDescriptorSet::Create({descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Frame Data Set Global"));
                        Graphics::RHI::Get().setFrameDataSet(m_FrameDataSet);
                    }
                });
        }

        //--------------------------------------------------------------------------

        void RZWorldRenderer::uploadLightsData(RZScene* scene, RZRendererSettings& settings)
        {
            m_Blackboard.add<SceneLightsData>() = m_FrameGraph.addCallbackPass<SceneLightsData>(
                "Scene Lights Data Upload",
                [&](FrameGraph::RZFrameGraph::RZBuilder& builder, SceneLightsData& data) {
                    builder.setAsStandAlonePass();

                    data.lightsDataBuffer = builder.create<FrameGraph::RZFrameGraphBuffer>("Scene Lights Data", {"Scene Lights Data", sizeof(GPULightsData)});
                    data.lightsDataBuffer = builder.write(data.lightsDataBuffer);
                },
                [=](const SceneLightsData& data, FrameGraph::RZFrameGraphPassResources& resources, void* rendercontext) {
                    GPULightsData gpuLightsData{};

                    auto& registry = scene->getRegistry();

                    // Upload the lights data after updating some stuff such as position etc.
                    auto group = scene->getRegistry().group<LightComponent>(entt::get<TransformComponent>);
                    for (auto entity: group) {
                        const auto& [lightComponent, transformComponent] = group.get<LightComponent, TransformComponent>(entity);

                        // Set the Position of the light using this transform component
                        lightComponent.light.getLightData().position = transformComponent.Translation;
                        lightComponent.light.setDirection(glm::vec3(glm::degrees(transformComponent.Rotation.x), glm::degrees(transformComponent.Rotation.y), glm::degrees(transformComponent.Rotation.z)));
                        gpuLightsData.lightData[gpuLightsData.numLights] = lightComponent.light.getLightData();

                        gpuLightsData.numLights++;
                    }
                    // update and upload the UBO
                    auto lightsDataBuffer = resources.get<FrameGraph::RZFrameGraphBuffer>(data.lightsDataBuffer).getHandle();
                    lightsDataBuffer->SetData(sizeof(GPULightsData), &gpuLightsData);

                    if (!Graphics::RHI::Get().getSceneLightsDataSet()) {
                        RZDescriptor lightsData_descriptor{};
                        lightsData_descriptor.bindingInfo.binding = 0;
                        lightsData_descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        lightsData_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        lightsData_descriptor.uniformBuffer       = lightsDataBuffer;

                        auto m_SceneLightsDataDescriptorSet = RZDescriptorSet::Create({lightsData_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Scene Lights Set Global"));
                        Graphics::RHI::Get().setSceneLightsDataSet(m_SceneLightsDataDescriptorSet);
                    }
                });
        }
    }    // namespace Graphics
}    // namespace Razix