#pragma once

#include "Razix/Core/Core.h"
#include "Razix/Core/Log.h"

#include "Razix/Core/OS/Window.h"

namespace Razix
{
    namespace Graphics {

        /* The Render API to use for rendering the application */
        enum class RenderAPI
        {
            OPENGL = 0,
            VULKAN = 1,
            DIRECTX11 = 2,
            DIRECTX12 = 3,  // Not Supported yet!
            GXM = 4,        // Not Supported yet! (PSVita)
            GCM = 5         // Not Supported yet! (PS3)
        };

        /**
         * The Graphics Context that manages the context of the underlying graphics API
         */
        class RAZIX_API RZGraphicsContext
        {
        public:
            static void Create(const WindowProperties& properties, RZWindow* window);
            static void Release();

            virtual void Init() = 0;
            //! Only here the Destroy method is in PascalCase rest of the API must maintain a camelCase destroy
            virtual void Destroy() = 0;
            // TODO: Remove this!
            virtual void SwapBuffers() = 0;
            virtual void ClearWithColor(float r, float g, float b) = 0;

            static RZGraphicsContext* GetContext();

            static RenderAPI GetRenderAPI() { return s_RenderAPI; }
            static void SetRenderAPI(RenderAPI api) { s_RenderAPI = api; }
            static const std::string GetRenderAPIString();

        protected:
            /* Pointer to the underlying graphics APi implementation */
            static RZGraphicsContext* s_Context;
            /* The render API selected to render the application */
            static RenderAPI s_RenderAPI;
        };
    }
}