#pragma once

#include "Razix/Core/RZProfiling.h"

#include "Razix/Graphics/RHI/API/RZAPIHandles.h"
#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        class RZCommandBuffer;
        class RZPipeline;
        class RZDescriptorSet;
        class RZSwapchain;
        class RZDescriptorSet;
        class RZSemaphore;
        struct RZPushConstant;

        enum class DataType
        {
            FLOAT,
            UNSIGNED_INT,
            UNSIGNED_BYTE
        };

        /* Gives information the attachment clear Info */
        struct RAZIX_MEM_ALIGN AttachmentClearInfo
        {
            bool      clear = true; /* Whether or not to clear the particular attachment                                                */
            glm::vec4 clearColor;   /* Clear color with which the attachment is cleared, Note: x and y represent the depth clear values */
        };

        struct RenderingInfo
        {
            glm::uvec2                                                   extent;
            std::vector<std::pair<RZTextureHandle, AttachmentClearInfo>> colorAttachments;
            std::pair<RZTextureHandle, AttachmentClearInfo>              depthAttachment;
            int                                                          layerCount = 1;
            bool                                                         resize     = false;
        };

        /* Command Queue is a collection of command buffers that will be submitted for execution at once */
        typedef std::vector<RZCommandBuffer*> CommandQueue;

        /* The Razix RHI (Render Hardware Interface ) provides a interface and a set of common methods that abstracts over other APIs rendering implementation
         * The Renderers creates from the provided IRZRenderer interface of razix uses this to perform command recording/submission sets binding
         * and other operations that doesn't require the underlying API, since renderers do not actually need that we use this high-level abstraction
         * over the supported APIs to make things look simple and easier to interact with
         */
        class RAZIX_API RHI : public RZRoot
        {
        public:
            RHI()          = default;
            virtual ~RHI() = default;

            static void Create(u32 width, u32 height);
            static void Release();

            static RHI&       Get() { return *s_APIInstance; }
            static const RHI* GetPointer() { return s_APIInstance; }

            /* Initializes the API renderer with the resources it needs */
            RAZIX_FORCE_INLINE static void Init()
            {
                RAZIX_PROFILE_GPU_SCOPE("Init Rendering");
                s_APIInstance->InitAPIImpl();
            }
            RAZIX_FORCE_INLINE static void AcquireImage(RZSemaphore* signalSemaphore)
            {
                RAZIX_PROFILE_GPU_SCOPE("Acquire swap image");
                s_APIInstance->AcquireImageAPIImpl(signalSemaphore);
            }
            /* Begins recording of the command buffer */
            RAZIX_FORCE_INLINE static void Begin(RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Begin Rendering");
                s_APIInstance->BeginAPIImpl(cmdBuffer);
            }
            /* Submits the recorded command buffers onto the RenderQueue to be submitted to the GPU for execution */
            RAZIX_FORCE_INLINE static void Submit(RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Submit To Render Queue");
                s_APIInstance->SubmitImpl(cmdBuffer);
            }
            /* Executes the the lists of command buffers stacked up in the Command Queue  */
            RAZIX_FORCE_INLINE static void SubmitWork(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)
            {
                RAZIX_PROFILE_GPU_SCOPE("SubmitWork");
                s_APIInstance->SubmitWorkImpl(waitSemaphores, signalSemaphores);
            }
            /* Presents the rendered images to the presentation queue for the user to view */
            RAZIX_FORCE_INLINE static void Present(RZSemaphore* waitSemaphore)
            {
                RAZIX_PROFILE_GPU_SCOPE("Present Swapchain");
                s_APIInstance->PresentAPIImpl(waitSemaphore);
            }
            RAZIX_FORCE_INLINE static void BindDescriptorSet(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Set at Idx");
                s_APIInstance->BindDescriptorSetAPImpl(pipeline, cmdBuffer, descriptorSet, setIdx);
            }
            RAZIX_FORCE_INLINE static void BindUserDescriptorSets(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Sets");
                s_APIInstance->BindUserDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets);
            }
            RAZIX_FORCE_INLINE static void BindUserDescriptorSets(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Sets");
                s_APIInstance->BindUserDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets, totalSets);
            }
            RAZIX_FORCE_INLINE static void BindPushConstant(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Pushconstants");
                s_APIInstance->BindPushConstantsAPIImpl(pipeline, cmdBuffer, pushConstant);
            }
            RAZIX_FORCE_INLINE static void Draw(RZCommandBuffer* cmdBuffer, u32 count, DataType dataType = DataType::UNSIGNED_INT)
            {
                RAZIX_PROFILE_GPU_SCOPE("Draw");
                s_APIInstance->DrawAPIImpl(cmdBuffer, count, dataType);
            }
            RAZIX_FORCE_INLINE static void DrawIndexed(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0)
            {
                RAZIX_PROFILE_GPU_SCOPE("Draw Indexed");
                s_APIInstance->DrawIndexedAPIImpl(cmdBuffer, indexCount);
            }
            RAZIX_FORCE_INLINE static void OnResize(u32 width, u32 height)
            {
                RAZIX_PROFILE_GPU_SCOPE("OnResize");
                s_APIInstance->OnResizeAPIImpl(width, height);
            }

            RAZIX_FORCE_INLINE static void BeginRendering(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)
            {
                RAZIX_PROFILE_GPU_SCOPE("Begin Rendering");
                s_APIInstance->BeginRenderingImpl(cmdBuffer, renderingInfo);
            }

            RAZIX_FORCE_INLINE static void EndRendering(RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("End Rendering");
                s_APIInstance->EndRenderingImpl(cmdBuffer);
            }
            RAZIX_FORCE_INLINE static void EnableBindlessTextures(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer)
            {
                RAZIX_PROFILE_GPU_SCOPE("Enable Bindless Textures");
                s_APIInstance->EnableBindlessTexturesImpl(pipeline, cmdBuffer);
            }
            RAZIX_FORCE_INLINE static void BindPushDescriptors(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Push Descriptors");
                s_APIInstance->BindPushDescriptorsImpl(pipeline, cmdBuffer, descriptors);
            }

            RAZIX_FORCE_INLINE static void SetDepthBias(RZCommandBuffer* cmdBuffer)
            {
                return s_APIInstance->SetDepthBiasImpl(cmdBuffer);
            }
            RAZIX_FORCE_INLINE static void SetScissorRect(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height) { return s_APIInstance->SetScissorRectImpl(cmdBuffer, x, y, width, height); }

            RAZIX_FORCE_INLINE static RZSwapchain*     GetSwapchain() { return s_APIInstance->GetSwapchainImpl(); }
            RAZIX_FORCE_INLINE static RZCommandBuffer* GetCurrentCommandBuffer() { return s_APIInstance->m_CurrentCommandBuffer; }

            RAZIX_FORCE_INLINE const u32& getWidth() { return m_Width; }
            RAZIX_FORCE_INLINE const u32& getHeight() { return m_Height; }

            RAZIX_FORCE_INLINE static void SetCmdBuffer(RZCommandBuffer* cmdBuf) { s_APIInstance->m_CurrentCommandBuffer = cmdBuf; }

            static void SetCmdCheckpoint(RZCommandBuffer* cmdbuffer, void* markerData)
            {
                s_APIInstance->SetCmdCheckpointImpl(cmdbuffer, markerData);
            }

            const RZDescriptorSet* getFrameDataSet() const { return m_FrameDataSet; }
            void                   setFrameDataSet(RZDescriptorSet* set) { m_FrameDataSet = set; }

            const RZDescriptorSet* getSceneLightsDataSet() const { return m_SceneLightsDataSet; }
            void                   setSceneLightsDataSet(RZDescriptorSet* set) { m_SceneLightsDataSet = set; }

        protected:
            virtual void InitAPIImpl()                                                                                                                                              = 0;
            virtual void AcquireImageAPIImpl(RZSemaphore* signalSemaphore)                                                                                                          = 0;
            virtual void BeginAPIImpl(RZCommandBuffer* cmdBuffer)                                                                                                                   = 0;
            virtual void SubmitImpl(RZCommandBuffer* cmdBuffer)                                                                                                                     = 0;
            virtual void SubmitWorkImpl(std::vector<RZSemaphore*> waitSemaphores, std::vector<RZSemaphore*> signalSemaphores)                                                       = 0;
            virtual void PresentAPIImpl(RZSemaphore* waitSemaphore)                                                                                                                 = 0;
            virtual void BindDescriptorSetAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet* descriptorSet, u32 setIdx)                                = 0;
            virtual void BindUserDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptorSet*>& descriptorSets)                        = 0;
            virtual void BindUserDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const RZDescriptorSet** descriptorSets, u32 totalSets)                      = 0;
            virtual void BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)                                                    = 0;
            virtual void DrawAPIImpl(RZCommandBuffer* cmdBuffer, u32 count, DataType datayType = DataType::UNSIGNED_INT)                                                            = 0;
            virtual void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, u32 indexCount, u32 instanceCount = 1, u32 firstIndex = 0, int32_t vertexOffset = 0, u32 firstInstance = 0) = 0;
            virtual void DestroyAPIImpl()                                                                                                                                           = 0;
            virtual void OnResizeAPIImpl(u32 width, u32 height)                                                                                                                     = 0;
            virtual void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)                                                                                                               = 0;
            virtual void SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, u32 width, u32 height)                                                                = 0;
            virtual void EnableBindlessTexturesImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer)                                                                               = 0;
            virtual void BindPushDescriptorsImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, const std::vector<RZDescriptor>& descriptors)                                    = 0;
            virtual void BeginRenderingImpl(RZCommandBuffer* cmdBuffer, const RenderingInfo& renderingInfo)                                                                         = 0;
            virtual void EndRenderingImpl(RZCommandBuffer* cmdBuffer)                                                                                                               = 0;

            virtual RZSwapchain* GetSwapchainImpl() = 0;

            virtual void SetCmdCheckpointImpl(RZCommandBuffer* cmdbuffer, void* markerData) {}

        protected:
            static RHI* s_APIInstance;

            std::string                   m_RendererTitle; /* The name of the renderer API that is being used */
            u32                           m_Width      = 0;
            u32                           m_Height     = 0;
            u32                           m_PrevWidth  = 0;
            u32                           m_PrevHeight = 0;
            CommandQueue                  m_CommandQueue; /* The queue of recorded commands that needs execution */
            RZCommandBuffer*              m_CurrentCommandBuffer = nullptr;
            std::vector<RZCommandBuffer*> m_DrawCommandBuffers;
            RZDescriptorSet*              m_FrameDataSet       = nullptr;
            RZDescriptorSet*              m_SceneLightsDataSet = nullptr;
        };
    }    // namespace Graphics
}    // namespace Razix
