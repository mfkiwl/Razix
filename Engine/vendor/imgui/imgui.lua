project "ImGui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

     includedirs
        {
             "./"
        }

    files
    {
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp",
        "imgui_tables.cpp",
        "plugins/**.h",
        "plugins/**.cpp",
        "plugins/**.inl"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "off"

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
