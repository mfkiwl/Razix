project "spdlog"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

    files { "src/*.h", "src/*.cpp" }

	filter "configurations:Debug"
        defines { "RAZIX_DEBUG", "_DEBUG" }
        symbols "On"
        runtime "Debug"
        optimize "Off"

    filter "configurations:Release"
        defines { "RAZIX_RELEASE", "NDEBUG" }
        optimize "Speed"
        symbols "On"
        runtime "Release"

    filter "configurations:Distribution"
        defines { "RAZIX_DISTRIBUTION", "NDEBUG" }
        symbols "Off"
        optimize "Full"
        runtime "Release"
