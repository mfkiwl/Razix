IncludeDir = {}
IncludeDir["cereal"]            = "%{wks.location}/../Engine/" .. "vendor/cereal/include"
IncludeDir["Glad"]              = "%{wks.location}/../Engine/" .. "vendor/glad/include/"
IncludeDir["GLFW"]              = "%{wks.location}/../Engine/" .. "vendor/glfw/include/"
IncludeDir["ImGui"]             = "%{wks.location}/../Engine/" .. "vendor/imgui/"
IncludeDir["spdlog"]            = "%{wks.location}/../Engine/" .. "vendor/spdlog/include"
IncludeDir["stb"]               = "%{wks.location}/../Engine/" .. "vendor/stb/"
IncludeDir["glm"]               = "%{wks.location}/../Engine/" .. "vendor/glm/"
IncludeDir["SPIRVReflect"]      = "%{wks.location}/../Engine/" .. "vendor/SPIRVReflect/"
IncludeDir["SPIRVCross"]        = "%{wks.location}/../Engine/" .. "vendor/SPIRVCross/include"
IncludeDir["entt"]              = "%{wks.location}/../Engine/" .. "vendor/entt/include"
IncludeDir["lua"]               = "%{wks.location}/../Engine/" .. "vendor/lua/src"
IncludeDir["tracy"]             = "%{wks.location}/../Engine/" .. "vendor/tracy"
IncludeDir["optick"]            = "%{wks.location}/../Engine/" .. "vendor/optick/src"
IncludeDir["simde"]             = "%{wks.location}/../Engine/" .. "vendor/simde"
IncludeDir["Jolt"]              = "%{wks.location}/../Engine/" .. "vendor/JoltPhysics"

IncludeDir["Razix"]             = "%{wks.location}/../Engine/" .. "src"
IncludeDir["vendor"]            = "%{wks.location}/../Engine/" .. "vendor/"

-- Vulkan SDK
VulkanSDK = os.getenv("VULKAN_SDK")

if (VulkanSDK == nil or VulkanSDK == '') then
    print("VULKAN_SDK Enviroment variable is not found! Please check your development environment settings")
    os.exit()
else
    print("Vulkan SDK found at : " .. VulkanSDK)
end