#pragma once

#include "Razix/Core/RZProfiling.h"

#include "Razix/Graphics/API/RZDescriptorSet.h"

namespace Razix {
    namespace Graphics {

        class RZCommandBuffer;
        class RZPipeline;
        class RZDescriptorSet;
        class RZSwapchain;
        class RZDescriptorSet;
        struct RZPushConstant;

        enum class DataType
        {
            FLOAT,
            UNSIGNED_INT,
            UNSIGNED_BYTE
        };

        typedef std::vector<RZCommandBuffer*> CommandQueue;

        /* The Razix API Renderer provides a interface and a set of common methods that abstracts over other APIs rendering implementation
         * The Renderers creates from the provided IRZRenderer interface of razix uses this to perform command recording/submission sets binding
         * and other operations that doesn't require the underlying API, since renderers do not actually need that we use this high-level abstraction
         * over the supported APIs to make things look simple and easier to interact with
         */
        class RAZIX_API RZRenderContext : public RZRoot
        {
        public:
            RZRenderContext()          = default;
            virtual ~RZRenderContext() = default;

            static void Create(uint32_t width, uint32_t height);
            static void Release();

            static RZRenderContext& Get() { return *s_APIInstance; }

            /* Initializes the API renderer with the resources it needs */
            RAZIX_FORCE_INLINE static void Init()
            {
                RAZIX_PROFILE_GPU_SCOPE("Init Rendering");
                s_APIInstance->InitAPIImpl();
            }
            RAZIX_FORCE_INLINE static void AcquireImage()
            {
                RAZIX_PROFILE_GPU_SCOPE("Acquire swap image");
                s_APIInstance->AcquireImageAPIImpl();
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
            RAZIX_FORCE_INLINE static void SubmitWork()
            {
                RAZIX_PROFILE_GPU_SCOPE("SubmitWork");
                s_APIInstance->SubmitWorkImpl();
            }
            /* Presents the rendered images to the presentation queue for the user to view */
            RAZIX_FORCE_INLINE static void Present()
            {
                RAZIX_PROFILE_GPU_SCOPE("Submit and Present"); /*RAZIX_PROFILE_GPU_FLIP(getSwapchain()->GetAPIHandle());*/
                s_APIInstance->PresentAPIImpl();
            }
            RAZIX_FORCE_INLINE static void BindDescriptorSets(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Sets");
                s_APIInstance->BindDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets);
            }
            RAZIX_FORCE_INLINE static void BindDescriptorSets(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Descriptor Sets");
                s_APIInstance->BindDescriptorSetsAPImpl(pipeline, cmdBuffer, descriptorSets, totalSets);
            }
            RAZIX_FORCE_INLINE static void BindPushConstant(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)
            {
                RAZIX_PROFILE_GPU_SCOPE("Bind Pushconstants");
                s_APIInstance->BindPushConstantsAPIImpl(pipeline, cmdBuffer, pushConstant);
            }
            RAZIX_FORCE_INLINE static void Draw(RZCommandBuffer* cmdBuffer, uint32_t count, DataType dataType = DataType::UNSIGNED_INT)
            {
                RAZIX_PROFILE_GPU_SCOPE("Draw");
                s_APIInstance->DrawAPIImpl(cmdBuffer, count, dataType);
            }
            RAZIX_FORCE_INLINE static void DrawIndexed(RZCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0)
            {
                RAZIX_PROFILE_GPU_SCOPE("Draw Indexed");
                s_APIInstance->DrawIndexedAPIImpl(cmdBuffer, indexCount);
            }
            RAZIX_FORCE_INLINE static void OnResize(uint32_t width, uint32_t height)
            {
                RAZIX_PROFILE_GPU_SCOPE("OnResize");
                s_APIInstance->OnResizeAPIImpl(width, height);
            }

            RAZIX_FORCE_INLINE static void SetDepthBias(RZCommandBuffer* cmdBuffer) { return s_APIInstance->SetDepthBiasImpl(cmdBuffer); }
            RAZIX_FORCE_INLINE static void SetScissorRect(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, uint32_t width, uint32_t height) { return s_APIInstance->SetScissorRectImpl(cmdBuffer, x, y, width, height); }

            RAZIX_FORCE_INLINE static RZSwapchain*     getSwapchain() { return s_APIInstance->GetSwapchainImpl(); }
            RAZIX_FORCE_INLINE static RZCommandBuffer* getCurrentCommandBuffer() { return s_APIInstance->m_CurrentCommandBuffer; }

            RAZIX_FORCE_INLINE const uint32_t& getWidth() { return m_Width; }
            RAZIX_FORCE_INLINE const uint32_t& getHeight() { return m_Height; }

        protected:
            virtual void InitAPIImpl()                                                                                                                                                                  = 0;
            virtual void AcquireImageAPIImpl()                                                                                                                                                          = 0;
            virtual void BeginAPIImpl(RZCommandBuffer* cmdBuffer)                                                                                                                                       = 0;
            virtual void SubmitImpl(RZCommandBuffer* cmdBuffer)                                                                                                                                         = 0;
            virtual void SubmitWorkImpl()                                                                                                                                                               = 0;
            virtual void PresentAPIImpl()                                                                                                                                                               = 0;
            virtual void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, std::vector<RZDescriptorSet*>& descriptorSets)                                                      = 0;
            virtual void BindDescriptorSetsAPImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZDescriptorSet** descriptorSets, uint32_t totalSets)                                               = 0;
            virtual void BindPushConstantsAPIImpl(RZPipeline* pipeline, RZCommandBuffer* cmdBuffer, RZPushConstant pushConstant)                                                                        = 0;
            virtual void DrawAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t count, DataType datayType = DataType::UNSIGNED_INT)                                                                           = 0;
            virtual void DrawIndexedAPIImpl(RZCommandBuffer* cmdBuffer, uint32_t indexCount, uint32_t instanceCount = 1, uint32_t firstIndex = 0, int32_t vertexOffset = 0, uint32_t firstInstance = 0) = 0;
            virtual void DestroyAPIImpl()                                                                                                                                                               = 0;
            virtual void OnResizeAPIImpl(uint32_t width, uint32_t height)                                                                                                                               = 0;
            virtual void SetDepthBiasImpl(RZCommandBuffer* cmdBuffer)                                                                                                                                   = 0;
            virtual void SetScissorRectImpl(RZCommandBuffer* cmdBuffer, int32_t x, int32_t y, uint32_t width, uint32_t height)                                                                          = 0;

            virtual RZSwapchain* GetSwapchainImpl() = 0;

        protected:
            static RZRenderContext* s_APIInstance;

            std::string      m_RendererTitle; /* The name of the renderer API that is being used */
            uint32_t         m_Width;
            uint32_t         m_Height;
            CommandQueue     m_CommandQueue; /* The queue of recorded commands that needs execution */
            RZCommandBuffer* m_CurrentCommandBuffer;
        };
    }    // namespace Graphics
}    // namespace Razix
