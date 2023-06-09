find_package(Vulkan)

if (NOT Vulkan_FOUND)
	MESSAGE(STATUS "Could not locate Vulkan SDK! Vulkan will be disabled")
	return()
endif()

Project(Vulkan)


set(sources "")
FolderSources(sources "" src)
if (NOT ANDROID AND NOT EMSCRIPTEN AND NOT IOS)
	FolderSources(sources src glad vulkan.c vulkan.h)
endif()
FolderSources(sources src Vulkan vulkanforward.h vulkanlib.h vulkanmeshdata.h vulkanmeshloader.cpp vulkanmeshloader.h vulkanpipelineloader.cpp vulkanpipelineloader.h vulkanrendertexture.cpp vulkanrendertexture.h vulkanrenderwindow.cpp vulkanrenderwindow.h vulkantextureloader.cpp vulkantextureloader.h vulkanrendertarget.cpp vulkanrendertarget.h vulkanrendercontext.cpp vulkanrendercontext.h vulkanshadercodegen.cpp vulkanshadercodegen.h vulkanshaderloader.cpp vulkanshaderloader.h)
	FolderSources(sources Vulkan util vulkanbuffer.cpp vulkanbuffer.h vulkancommandlist.cpp vulkancommandlist.h vulkanconstantbufferheap.cpp vulkanconstantbufferheap.h vulkantexture.cpp vulkantexture.h vulkanpipeline.cpp vulkanpipeline.h vulkanpipelineinstance.cpp vulkanpipelineinstance.h)

add_plugin(Vulkan Madgine Renderer ${sources} PRECOMPILED_HEADER src/Vulkan/vulkanlib.h)

target_link_libraries(Vulkan PRIVATE Client Vulkan::Vulkan)

if (WINDOWS)
	target_compile_definitions(Vulkan PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif (WINDOWS)

set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools Vulkan_Tools vulkantoolconfig.cpp vulkantoolconfig.h vulkantoolsforward.h vulkantoolslib.h)
		FolderSources(sources Vulkan_Tools imgui imgui_impl_vulkan.cpp imgui_impl_vulkan.h vulkanimroot.cpp vulkanimroot.h)


add_tools(Vulkan ${sources} PRECOMPILED_HEADER tools/Vulkan_Tools/vulkantoolslib.h)

target_link_plugins(VulkanTools ClientTools)
target_link_libraries(VulkanTools PRIVATE Vulkan::Vulkan)

add_shadergen_target(Vulkan -SPIRV)


if (ANDROID)
	list(APPEND ANDROID_DEPENDENCIES ${CMAKE_CURRENT_SOURCE_DIR}/ext/android_validation_layer;${ANDROID_DEPENDENCIES})
	set(ANDROID_DEPENDENCIES "${ANDROID_DEPENDENCIES}" CACHE INTERNAL "")
endif ()

