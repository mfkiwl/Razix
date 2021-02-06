#pragma once

#include "Core.h"
#include "Window.h"
#include "Razix/LayerStack.h"
#include "Razix/Events/ApplicationEvent.h"
#include "Razix/Events/KeyEvent.h"
#include "Razix/Events/MouseEvent.h"

namespace Razix
{
    // Create an Razix Application
    class RAZIX_API Application
    {
    public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& event);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        inline Window& GetWindow() { return *m_Window; }
        inline static Application& GetApplication() { return sInstance; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;
    private:
        Application* sInstance;
    };

    // To be defined on CLIENT side to create the application
    //
    // What this means is that the Entry point thinks the engine will define this for sure.
    // In fact it does by just forward declaring, but who actually defines it?
    // Now the engine forces the client to implement this according to their needs. 
    // [Application(forward declaration)-->Entry Point(extern declaration)-->CLIENT(definition)]
    Application* CreateApplication();
}

 