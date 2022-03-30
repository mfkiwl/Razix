#pragma once

#include "Razix/Core/RZCore.h"
#include "Razix/Core/RZLog.h"

#include "Razix/Core/OS/RZWindow.h"

namespace Razix
{
    namespace Graphics {

        /* The Render API to use for rendering the application */
        enum class RenderAPI
        {
            NONE        = -1,
            OPENGL      = 0,
            VULKAN      = 1,
            METAL       = 2,    // MacOS and iOS only
            DIRECTX11   = 3,    // To be removed in favour of a single low-level API OpenGL
            DIRECTX12   = 4,    // Not Supported yet!
            GXM         = 5,    // Not Supported yet! (PSVita)
            GCM         = 6,    // Not Supported yet! (PS3)
            GNM         = 7,    // Not Supported yet! (PS4)
            GNMX        = 8,    // Not Supported yet! (PS4 abstracted API)
            AGC         = 9     // Not Supported yet! (PS5)
        };

        /**
         * The Graphics Context that manages the context of the underlying graphics API
         */
        class RAZIX_API RZGraphicsContext
        {
        public:
            /**
             * Creates the Graphics context with the underlying API
             * 
             * @param properties The window properties with which the window was created with
             * @param window The window handle to which the Graphics will be rendered to
             */
            static void Create(const WindowProperties& properties, RZWindow* window);
            /* Release the graphics context object and it's resources */
            static void Release();

            /* Initializes the underlying Graphics API for rendering */
            virtual void Init() = 0;
            //! Only here the Destroy method is in PascalCase rest of the API must maintain a camelCase destroy
            virtual void Destroy() = 0;
            /* Clears the screen with the given color */
            virtual void ClearWithColor(float r, float g, float b) = 0;

            /* Returns the pointer to the underlying graphics API */
            static RZGraphicsContext* GetContext();

            /* Gets the current render API being used by the engine to render */
            static RenderAPI GetRenderAPI() { return s_RenderAPI; }
            /* Set the render API to use to render */
            static void SetRenderAPI(RenderAPI api) { s_RenderAPI = api; }
            /* Gets the render API being used as a string */
            static const std::string GetRenderAPIString();

        protected:
            /* Pointer to the underlying graphics APi implementation */
            static RZGraphicsContext* s_Context;
            /* The render API selected to render the application */
            static RenderAPI s_RenderAPI;
        };
    }
}
