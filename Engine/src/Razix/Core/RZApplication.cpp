// clang-format off
#include "rzxpch.h"
// clang-format on

#include "RZApplication.h"

// ---------- Engine ----------
#include "Razix/Core/RZEngine.h"
// ----------------------------

#include "Razix/Core/RZSplashScreen.h"
#include "Razix/Core/RazixVersion.h"

#include "Razix/Core/OS/RZInput.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"

#include "Razix/Events/ApplicationEvent.h"

#include "Razix/Graphics/API/RZAPIRenderer.h"
#include "Razix/Graphics/API/RZGraphicsContext.h"
#include "Razix/Graphics/API/RZSwapchain.h"
#include "Razix/Graphics/API/RZTexture.h"

#include "Razix/Scene/Components/CameraComponent.h"

#include "Razix/Graphics/Renderers/RZImGuiRenderer.h"

#include <backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <cereal/archives/json.hpp>

namespace Razix {
    RZApplication* RZApplication::s_AppInstance = nullptr;

    bool                    RZApplication::ready_for_execution = false;
    std::mutex              RZApplication::m;
    std::condition_variable RZApplication::halt_execution;

    RZApplication::RZApplication(const std::string& projectRoot, const std::string& appName /*= "Razix App"*/)
        : m_AppName(appName), m_Timestep(RZTimestep(0.0f))
    {
        // Create the application instance
        RAZIX_CORE_ASSERT(!s_AppInstance, "Application already exists!");
        s_AppInstance = this;

        // Set the Application root path and Load the project settings
        const std::string& razixRoot = RAZIX_STRINGIZE(RAZIX_ROOT_DIR);
        // Path to the Project path (*.razixproject)
        // TODO: Since the Engine will be installed elsewhere and and Project will be else where this logic has to be re-factored to use the proper project path to resolve the VFS to mount the project Assets
        m_AppFilePath = razixRoot + projectRoot;
        RAZIX_CORE_TRACE("Project file path : {0}", m_AppFilePath);

        Razix::RZSplashScreen::Get().setLogString("Loading Shader Cache...");
        Razix::RZSplashScreen::Get().setLogString("Loading Project Assets..");

        // Mount the VFS paths based on the Project directory (done here cause the Application can make things easier by making this easy by loading some default directories, others can be added later sandbox shouldn't be troubled by all this labor work)
        // First the default sandbox or sample project is loaded that is provided by the engine that resides with the engine
        // Next it checks the command line for the project file directory
        // Project root directory
        RAZIX_CORE_TRACE("Mounting file systems... for Project at : {0}", m_AppFilePath);
        Razix::RZSplashScreen::Get().setLogString("Mounting file systems...");

        RZVirtualFileSystem::Get().mount("Project", m_AppFilePath);

        RZVirtualFileSystem::Get().mount("Assets", m_AppFilePath + std::string("Assets"));
        RZVirtualFileSystem::Get().mount("Meshes", m_AppFilePath + std::string("Assets/Meshes"));
        RZVirtualFileSystem::Get().mount("Scenes", m_AppFilePath + std::string("Assets/Scenes"));
        RZVirtualFileSystem::Get().mount("Scripts", m_AppFilePath + std::string("Assets/Scripts"));
        RZVirtualFileSystem::Get().mount("Sounds", m_AppFilePath + std::string("Assets/Sounds"));
        RZVirtualFileSystem::Get().mount("Textures", m_AppFilePath + std::string("Assets/Textures"));
    }

    void RZApplication::Init()
    {
        // Load the De-serialized data from the project file or use the command line argument to open the file
        // TODO: Add verification for Engine and Project Version
        std::ifstream AppStream;
        if (RZEngine::Get().commandLineParser.isSet("project filename")) {
            std::string fullPath = RZEngine::Get().commandLineParser.getValueAsString("project filename");
            RAZIX_CORE_TRACE("Command line filename : {0}", fullPath);
            AppStream.open(fullPath, std::ifstream::in);
            m_AppFilePath = fullPath.substr(0, fullPath.find_last_of("\\/")) + "/";
        } else {
            // TODO: If command line is not provided or doesn't use engine default sandbox project we need some way to resolve the project root directory, make this agnostic we need not redirect to sandbox by default it must be provided as a placeholder value instead as a fall back
            //m_AppFilePath = ??
            std::string projectFullPath = m_AppFilePath + m_AppName + std::string(".razixproject");
            AppStream.open(projectFullPath, std::ifstream::in);
        }

        // Check the command line arguments for the rendering api
        if (RZEngine::Get().commandLineParser.isSet("rendering api"))
            Graphics::RZGraphicsContext::SetRenderAPI((Graphics::RenderAPI) RZEngine::Get().commandLineParser.getValueAsInt("project filename"));

        // De-serialize the application
        if (AppStream.is_open()) {
            RAZIX_CORE_TRACE("Loading project file...");
            cereal::JSONInputArchive inputArchive(AppStream);
            inputArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        // The Razix Application Signature Name is generated here and passed to the window
        std::string SignatureTitle = m_AppName + " | " + "Razix Engine" + " - " + Razix::RazixVersion.getVersionString() + " " + "[" + Razix::RazixVersion.getReleaseStageString() + "]" + " " + "<" + Graphics::RZGraphicsContext::GetRenderAPIString() + ">" + " | " + " " + RAZIX_STRINGIZE(RAZIX_BUILD_CONFIG);

        // Create the timer
        m_Timer = CreateUniqueRef<RZTimer>();

        // Set the window properties and create the timer
        m_WindowProperties.Title = SignatureTitle;

        // TODO: Load any other Engine systems that needs to be done only in the Application
        // Destroy the Splash Screen before we create the window
        Razix::RZSplashScreen::Get().destroy();

        // Create the Window only if it's not set before (using the native window pointer, usually done by the QT editor)
        if (m_Window == nullptr) {
            m_Window = RZWindow::Create(m_WindowProperties);
        }
        m_Window->SetEventCallback(RAZIX_BIND_CB_EVENT_FN(RZApplication::OnEvent));

        // Create a default project file file if nothing exists
        if (!AppStream.is_open()) {
            RAZIX_CORE_ERROR("Project File does not exist!");
            std::string               projectFullPath = m_AppFilePath + m_AppName + std::string(".razixproject");
            std::ofstream             opAppStream(projectFullPath);
            cereal::JSONOutputArchive defArchive(opAppStream);
            RAZIX_CORE_TRACE("Creating a default Project file...");

            defArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
        }

        // Convert the app to loaded state
        m_CurrentState = AppState::Running;

        // Enable V-Sync
        //m_Window->SetVSync(true);
    }

    void RZApplication::OnEvent(RZEvent& event)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        RZEventDispatcher dispatcher(event);
        // Window close event
        dispatcher.Dispatch<WindowCloseEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowClose));
        // Window resize event
        dispatcher.Dispatch<RZWindowResizeEvent>(RAZIX_BIND_CB_EVENT_FN(OnWindowResize));

        // Mouse Events
        // Mouse Moved event
        dispatcher.Dispatch<RZMouseMovedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseMoved));
        dispatcher.Dispatch<RZMouseButtonPressedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseButtonPressed));
        dispatcher.Dispatch<RZMouseButtonReleasedEvent>(RAZIX_BIND_CB_EVENT_FN(OnMouseButtonReleased));
    }

    bool RZApplication::OnWindowClose(WindowCloseEvent& e)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        m_CurrentState = AppState::Closing;
        return true;
    }

    bool RZApplication::OnWindowResize(RZWindowResizeEvent& e)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        //if (m_ImGuiRenderer != nullptr) {
        // Resize ImGui
        ImGuiIO& io                = ImGui::GetIO();
        io.DisplaySize             = ImVec2(e.GetWidth(), e.GetHeight());
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
        //}

        RZEngine::Get().getRenderStack().OnResize(e.GetWidth(), e.GetHeight());

        OnResize(e.GetWidth(), e.GetHeight());
        return true;
    }

    bool RZApplication::OnMouseMoved(RZMouseMovedEvent& e)
    {
        //if (m_ImGuiRenderer != nullptr) {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(e.GetX(), e.GetY());
        //}

        return true;
    }

    bool RZApplication::OnMouseButtonPressed(RZMouseButtonPressedEvent& e)
    {
        //if (m_ImGuiRenderer != nullptr) {
        ImGuiIO& io                          = ImGui::GetIO();
        io.MouseDown[e.GetMouseButton() - 1] = true;
        io.MouseDown[e.GetMouseButton() - 1] = true;
        //}

        return true;
    }

    bool RZApplication::OnMouseButtonReleased(RZMouseButtonReleasedEvent& e)
    {
        //if (m_ImGuiRenderer != nullptr) {
        ImGuiIO& io                          = ImGui::GetIO();
        io.MouseDown[e.GetMouseButton() - 1] = false;
        //}

        return true;
    }

    void RZApplication::Run()
    {
        // Create the API renderer to issue render commands
        Graphics::RZAPIRenderer::Create(getWindow()->getWidth(), getWindow()->getHeight());
        // TODO: Enable window V-Sync here
        Graphics::RZAPIRenderer::Init();

        // Job system and Engine Systems(run-time) Initialization
        Razix::RZEngine::Get().getRenderStack().PushRenderer(new Graphics::RZGridRenderer);
        Razix::RZEngine::Get().getRenderStack().PushRenderer(new Graphics::RZImGuiRenderer);

        // Now the scenes are loaded onto the scene manger here but they must be STATIC INITIALIZED shouldn't depend on the start up for the graphics context
        for (auto& sceneFilePath: sceneFilePaths)
            Razix::RZEngine::Get().getSceneManager().enqueueSceneFromFile(sceneFilePath);

        // Load a scene into memory
        Razix::RZEngine::Get().getSceneManager().loadScene(0);

        Start();

        albedoTexture = Graphics::RZTexture2D::CreateFromFile("//Textures/Avocado_baseColor.png", "Albedo", Graphics::RZTexture::Wrapping::CLAMP_TO_EDGE);
        albedoTexture->generateDescriptorSet();
        while (RenderFrame()) {}
        Quit();
        SaveApp();
    }

    bool RZApplication::RenderFrame()
    {
        RAZIX_PROFILE_FRAMEMARKER("RZApplication Main Thread");
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        // Calculate the delta time
        float now   = m_Timer->GetElapsedS();
        auto& stats = RZEngine::Get().GetStatistics();
        m_Timestep.Update(now);

        // Update the stats
        stats.FrameTime = m_Timestep.GetTimestepMs();
        //RAZIX_CORE_TRACE("Time steps : {0} ms", stats.FrameTime);

        // Poll for Input events
        m_Window->ProcessInput();

        // Early close if the escape key is pressed or close button is pressed
        if (RZInput::IsKeyPressed(Razix::KeyCode::Key::Escape))
            m_CurrentState = AppState::Closing;

        if (m_CurrentState == AppState::Closing)
            return false;

        // Update the Engine systems
        Update(m_Timestep);
        m_Updates++;

        // Render the Graphics
        Render();
        m_Frames++;

        // Update the window and it's surface/video out
        m_Window->OnWindowUpdate();

        // FLip the swapchain to present the rendered image
        //swapchain->Flip();

        {
            RAZIX_PROFILE_SCOPEC("RZApplication::TimeStepUpdates", RZ_PROFILE_COLOR_APPLICATION);

            // Record the FPS
            if (now - m_SecondTimer > 1.0f) {
                m_SecondTimer += 1.0f;

                stats.FramesPerSecond  = m_Frames;
                stats.UpdatesPerSecond = m_Updates;
                RAZIX_CORE_TRACE("FPS : {0}", stats.FramesPerSecond);
                //RAZIX_CORE_TRACE("UPS : {0} ms", stats.UpdatesPerSecond);

                m_Frames  = 0;
                m_Updates = 0;
            }
        }
        RZEngine::Get().ResetStats();
        return m_CurrentState != AppState::Closing;
    }

    void RZApplication::Start()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        OnStart();

        // Run the OnStart method for all the scripts in the scene
        if (RZEngine::Get().getSceneManager().getCurrentScene())
            RZEngine::Get().getScriptHandler().OnStart(RZEngine::Get().getSceneManager().getCurrentScene());
    }

    void RZApplication::Update(const RZTimestep& dt)
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        if (RZApplication::Get().getAppType() != AppType::GAME) {
            // Wait until Editor sends data
            std::unique_lock<std::mutex> lk(m);
            halt_execution.wait(lk, [] {
                return ready_for_execution;
            });
            // Manual unlocking is done before notifying, to avoid waking up
            // the waiting thread only to block again (see notify_one for details)
            lk.unlock();
            halt_execution.notify_one();
        }

        // TODO: Check if it's the primary or not and make sure you render only to the Primary Camera, if no primary camera don't render!!!!
        // Update the renderer stuff here
        RZEngine::Get().getSceneManager().getCurrentScene()->getSceneCamera().Camera.update(dt.GetTimestepMs());

        // Update ImGui
        ImGuiIO& io = ImGui::GetIO();
        (void) io;
        io.DisplaySize = ImVec2(getWindow()->getWidth(), getWindow()->getHeight());

        // Run the OnUpdate for all the scripts
        if (RZEngine::Get().getSceneManager().getCurrentScene())
            RZEngine::Get().getScriptHandler().OnUpdate(RZEngine::Get().getSceneManager().getCurrentScene(), dt);

        // Client App Update
        OnUpdate(dt);

#if 1
        if (Razix::Graphics::RZGraphicsContext::GetRenderAPI() == Razix::Graphics::RenderAPI::OPENGL)
            ImGui_ImplOpenGL3_NewFrame();

        // TODO: Well GLFW needs to be removed at some point and we need to use native functions
        if (RZApplication::Get().getAppType() != AppType::GAME)
            ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();

        OnImGui();

        ImGui::ShowDemoWindow();
        if (ImGui::Begin("Razix Engine")) {
            ImGui::Text("Indeed it is!");

            ImGui::Image((void*) albedoTexture->getDescriptorSet(), ImVec2(ImGui::GetWindowSize()[0], 400));
            static bool some;
            if (ImGui::Checkbox("Test", &some)) {
                RAZIX_CORE_ERROR("Done!");
            }
        }
        ImGui::End();

        ImGui::Render();

#endif
    }

    void RZApplication::Render()
    {
        RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_APPLICATION);

        // We are not checking if the current is scene is null or not
        Razix::RZEngine::Get().getRenderStack().BeginScene(RZEngine::Get().getSceneManager().getCurrentScene());

        Razix::RZEngine::Get().getRenderStack().OnRender();

        Razix::RZEngine::Get().getRenderStack().EndScene(RZEngine::Get().getSceneManager().getCurrentScene());

        OnRender();
    }

    void RZApplication::Quit()
    {
        albedoTexture->getDescriptorSet()->Destroy();
        albedoTexture->Release(true);

        Razix::RZEngine::Get().getRenderStack().Destroy();

        // Client side quit customization
        OnQuit();

        // Save the scene and the Application
        RZEngine::Get().getSceneManager().saveAllScenes();
        SaveApp();

        Graphics::RZAPIRenderer::Release();

        RAZIX_CORE_ERROR("Closing Application!");
    }

    void RZApplication::SaveApp()
    {
        // Save the app data before closing
        RAZIX_CORE_WARN("Saving project...");
        std::string               projectFullPath = m_AppFilePath + m_AppName + std::string(".razixproject");
        std::ofstream             opAppStream(projectFullPath);
        cereal::JSONOutputArchive saveArchive(opAppStream);
        saveArchive(cereal::make_nvp("Razix Application", *s_AppInstance));
    }
}    // namespace Razix