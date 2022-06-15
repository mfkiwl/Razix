project "optick"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    -- Vulkan SDK
    VulkanSDK = os.getenv("VULKAN_SDK")

    if (VulkanSDK == nil or VulkanSDK == '') then
        print("VULKAN_SDK Enviroment variable is not found! Please check your development environment settings")
        os.exit()
    else
        print("Vulkan SDK found at : " .. VulkanSDK)
    end

    files
    {
        "src/**.h",
        "src/**.hpp",
        "src/**.c",
        "src/**.cpp",
        "src/optick_gpu.d3d12.cpp",
        "src/optick_gpu.vulkan.cpp"
    }

    -- Needed when using clang-cl on windows as it will default to linux as soon as it sees clang is being used
    --defines "OPTICK_MSVC"

    filter "system:windows"
        systemversion "latest"

         -- Windows specific incldue directories
        includedirs
        {
             VulkanSDK .. "/include"
        }

        --buildoptions { "-Wno-narrowing" }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"

    filter "configurations:Distribution"
        runtime "Release"
        symbols "Off"
        optimize "Full"
