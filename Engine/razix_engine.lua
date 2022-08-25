-- Razix Engine vendor Common Inlcudes 
include 'Scripts/premake/common/vendor_includes.lua'
-- Internal libraies include dirs
include 'Scripts/premake/common/internal_includes.lua'

-- Vulkan SDK
VulkanSDK = os.getenv("VULKAN_SDK")

if (VulkanSDK == nil or VulkanSDK == '') then
    print("VULKAN_SDK Enviroment variable is not found! Please check your development environment settings")
    os.exit()
else
    print("Vulkan SDK found at : " .. VulkanSDK)
end

-- Razix project
project "Razix"
    kind "SharedLib"
    language "C++"

    pchheader "src/rzxpch.h"
    pchsource "src/rzxpch.cpp"

    -- Razix Engine defines (Global)
    defines
    {
        --Razix
        "RAZIX_ENGINE",
        "RAZIX_BUILD_DLL",
        "RAZIX_ROOT_DIR="  .. root_dir,
        "RAZIX_BUILD_CONFIG=" .. outputdir,
        -- Renderer
        "RAZIX_RENDERER_RAZIX",
        "RAZIX_RENDERER_FALCOR",
        "RAZIX_RAY_TRACE_RENDERER_RAZIX",
        "RAZIX_RAY_TRACE_RENDERER_OPTIX",
        "RAZIX_RAY_TRACE_RENDERER_EMBREE",
        -- vendor
        "OPTICK_MSVC"
    }

    -- Razix Engine source files (Global)
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp",
        "src/**.inl",
        -- vendor
        --"vendor/tracy/TracyClient.cpp",
        -- Shader files
        -- GLSL
        "content/Shaders/GLSL/*.vert",
        "content/Shaders/GLSL/*.frag",
        -- HLSL
        "content/Shaders/HLSL/*.hlsl",
        -- PSSL
        "content/Shaders/PSSL/*.pssl",
        "content/Shaders/PSSL/*.h",
        "content/Shaders/PSSL/*.hs",
        -- Cg
        "content/Shaders/CG/*.cg",
        -- Razix Shader Files
        "content/Shaders/Razix/*.rzsf"
    }

    -- Lazily add the platform files based on OS config
    removefiles
    {
        "src/Razix/Platform/**"
    }

    -- For MacOS
    sysincludedirs
    {
        -- Engine
        "./",
        "../",
        "internal/",
        "src/",
        "src/Razix",
        -- Vendor
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.cereal}",
        "%{IncludeDir.SPIRVReflect}",
        "%{IncludeDir.SPIRVCross}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.lua}",
        "%{IncludeDir.tracy}",
        "%{IncludeDir.optick}",
        "%{IncludeDir.Razix}",
        "%{IncludeDir.vendor}",
        -- API related
        "%{VulkanSDK}",
        -- Internal libraries
        "%{InternalIncludeDir.RazixMemory}"
    }

    -- Razix engine external linkage libraries (Global)
    links
    {
        "glfw",
        "imgui",
        "spdlog", -- Being linked staically by RazixMemory (Only include this in debug and release build exempt this in Distribution build)
        "SPIRVReflect",
        "SPIRVCross",
        "meshoptimizer",
        "OpenFBX", 
        "lua",
        "optick",
        "tracy",
        -- Razix Internal Libraries 
        -- 1. Razix Memory
        "RazixMemory"
    }

    -- TODO Add as rules, every shader file type will have it's own rule
    -- Don't build the shaders, they are compiled by the engine once and cached
   filter { "files:**.glsl or **.hlsl or **.pssl or **.cg or **.rzsf"}
        flags { "ExcludeFromBuild"}

    -- Build GLSL files based on their extension
    filter {"files:**.vert or **.frag"}
        removeflags "ExcludeFromBuild"
        buildmessage 'Compiling glsl shader : %{file.name}'
        buildcommands 'glslc.exe "%{file.directory}/%{file.name}" -o "%{file.directory}/../Compiled/SPIRV/%{file.name}.spv" '
        buildoutputs "%{file.directory}/../Compiled/SPIRV/%{file.name }.spv"

    -- Disable PCH for vendors
    filter 'files:vendor/**.cpp'
        flags  { 'NoPCH' }
    filter 'files:vendor/**.c'
        flags  { 'NoPCH' }

     -- Disable warning for vendor
    filter { "files:vendor/**"}
        warnings "Off"

    -- Razix Project settings for Windows
    filter "system:windows"
        cppdialect "C++17"
        staticruntime "off"
        systemversion "latest"
        disablewarnings { 4307 }
        characterset ("MBCS")
        editandcontinue "Off"

         -- Enable AVX, AVX2, Bit manipulation Instruction set (-mbmi)
         -- because GCC uses fused-multiply-add (fma) instruction by default, if it is available. Clang, on the contrary, doesn't use them by default, even if it is available, so we enable it explicityly
        -- Only works with GCC and Clang
        --buildoptions { "-mavx", "-mavx2", "-mbmi", "-march=haswell"}--, "-mavx512f -mavx512dq -mavx512bw -mavx512vbmi -mavx512vbmi2 -mavx512vl"}

        pchheader "rzxpch.h"
        pchsource "src/rzxpch.cpp"

        -- Windows specific defines
        defines
        {
            -- Engine
            "RAZIX_PLATFORM_WINDOWS",
            "RAZIX_USE_GLFW_WINDOWS",
            "RAZIX_IMGUI",
            -- API
            "RAZIX_RENDER_API_OPENGL",
            "RAZIX_RENDER_API_VULKAN",
            "RAZIX_RENDER_API_DIRECTX11",
            "RAZIX_RENDER_API_DIRECTX12",
            -- Windows / Vidual Studio
            "WIN32_LEAN_AND_MEAN",
            "_CRT_SECURE_NO_WARNINGS",
            "_DISABLE_EXTENDED_ALIGNED_STORAGE",
            "_SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING",
            "_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING",
            "TRACY_ENABLE"
        }

        -- Windows specific source files for compilation
        files
        {
            -- platform sepecific implementatioon
            "src/Razix/Platform/Windows/*.h",
            "src/Razix/Platform/Windows/*.cpp",

            "src/Razix/Platform/GLFW/*.h",
            "src/Razix/Platform/GLFW/*.cpp",

            -- Platform supported Graphics API implementatioon
            "src/Razix/Platform/API/OpenGL/*.h",
            "src/Razix/Platform/API/OpenGL/*.cpp",

            "src/Razix/Platform/API/Vulkan/*.h",
            "src/Razix/Platform/API/Vulkan/*.cpp",

            "src/Razix/Platform/API/DirectX11/*.h",
            "src/Razix/Platform/API/DirectX11/*.cpp",

            -- Vendor source files
            "vendor/glad/src/glad.c"
        }

        -- Windows specific incldue directories
        includedirs
        {
             VulkanSDK .. "/include"
        }

        -- Windows specific library directories
        libdirs
        {
            VulkanSDK .. "/Lib"
        }

        -- Windows specific linkage libraries (DirectX inlcude and library paths are implicityly added by Visual Studio, hence we need not add anything explicityly)
        links
        {
            "Dbghelp",
            -- Redner API
            "vulkan-1",
            "d3d11",
            "D3DCompiler"
        }

        -- Build options for Windows / Visual Studio (MSVC)
        buildoptions
        {
            "/MP", "/bigobj"
        }

    -- Config settings for Razix Engine project
    filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "DEBUG"}
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        optimize "Speed"
        symbols "On"

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION", "NDEBUG" }
        symbols "Off"
        optimize "Full"
