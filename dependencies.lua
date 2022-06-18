VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["Labyrinth"] 	= "%{wks.location}/Labyrinth/src"
IncludeDir["LabCore"] 		= "%{wks.location}/Labyrinth/src/Labyrinth/Core"
IncludeDir["RenderAPI"] 	= "%{wks.location}/Labyrinth/src/Labyrinth/Renderer/APIs"
IncludeDir["asio"] 			= "%{wks.location}/Labyrinth/dependencies/asio/include"
IncludeDir["box2d"] 		= "%{wks.location}/Labyrinth/dependencies/box2d/include"
IncludeDir["entt"] 			= "%{wks.location}/Labyrinth/dependencies/entt/include"
IncludeDir["glad"] 			= "%{wks.location}/Labyrinth/dependencies/glad/include"
IncludeDir["glfw"] 			= "%{wks.location}/Labyrinth/dependencies/glfw/include"
IncludeDir["glm"] 			= "%{wks.location}/Labyrinth/dependencies/glm"
IncludeDir["ImGui"] 		= "%{wks.location}/Labyrinth/dependencies/imgui"
IncludeDir["ImGuizmo"] 		= "%{wks.location}/Labyrinth/dependencies/ImGuizmo"
IncludeDir["lua"] 			= "%{wks.location}/Labyrinth/dependencies/lua"
IncludeDir["pfd"] 			= "%{wks.location}/Labyrinth/dependencies/portable-file-dialogs"
IncludeDir["rapidxml"] 		= "%{wks.location}/Labyrinth/dependencies/rapidxml"
IncludeDir["spdlog"] 		= "%{wks.location}/Labyrinth/dependencies/spdlog/include"
IncludeDir["stb_image"] 	= "%{wks.location}/Labyrinth/dependencies/stb_image"
IncludeDir["yaml_cpp"] 		= "%{wks.location}/Labyrinth/dependencies/yaml-cpp/include"
IncludeDir["shaderc"]   	= "%{wks.location}/Labyrinth/dependencies/shaderc/include"
IncludeDir["SPIRV_Cross"] 	= "%{wks.location}/Labyrinth/dependencies/SPIRV-Cross"
IncludeDir["VulkanSDK"] 	= "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] 		 	= "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] 	 	= "%{wks.location}/Labyrinth/dependencies/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] 	= "%{wks.location}/Labyrinth/dependencies/VulkanSDK/Bin"

Library = {}
Library["Vulkan"] 					= "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] 				= "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] 			= "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] 		= "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"]	= "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] 		= "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] 			= "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] 		= "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"