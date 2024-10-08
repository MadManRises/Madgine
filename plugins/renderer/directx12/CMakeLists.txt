find_package(WindowsSDK)

if (NOT WINDOWSSDK_FOUND)
	MESSAGE(STATUS "Could not locate Windows SDK! DirectX12 will be disabled")
	return()
endif()

Project(DirectX12)


set(sources "")
FolderSources(sources "" src)
FolderSources(sources src DirectX12 directx12forward.h directx12lib.h directx12geometryshaderloader.cpp directx12geometryshaderloader.h directx12meshdata.h directx12meshloader.cpp directx12meshloader.h directx12pixelshaderloader.cpp directx12pixelshaderloader.h directx12pipelineloader.cpp directx12pipelineloader.h directx12rendertexture.cpp directx12rendertexture.h directx12renderwindow.cpp directx12renderwindow.h directx12vertexshaderloader.cpp directx12vertexshaderloader.h directx12vertexshadersourceloader.cpp directx12vertexshadersourceloader.h directx12textureloader.cpp directx12textureloader.h directx12rendertarget.cpp directx12rendertarget.h directx12rendercontext.cpp directx12rendercontext.h directx12shadercodegen.cpp directx12shadercodegen.h)
	FolderSources(sources DirectX12 util dx12dump.cpp directx12buffer.cpp directx12buffer.h directx12constantbufferheap.cpp directx12constantbufferheap.h directx12descriptorheap.cpp directx12descriptorheap.h directx12texture.cpp directx12texture.h directx12pipeline.cpp directx12pipeline.h directx12pipelineinstance.cpp directx12pipelineinstance.h)

add_plugin(DirectX12 Madgine Renderer ${sources} PRECOMPILED_HEADER src/DirectX12/directx12lib.h)

get_windowssdk_library_dirs(${WINDOWSSDK_PREFERRED_DIR} lib_dirs)

list (GET lib_dirs 0 lib)

string (REPLACE "/lib/" "/bin/" bin ${lib})
string (REPLACE "/um/" "/" bin ${bin})

target_link_directories(DirectX12 PRIVATE ${lib})

target_link_libraries(DirectX12 PRIVATE d3d12 dxgi dxguid Client dxcompiler)

add_custom_command(TARGET DirectX12 POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${bin}/dxcompiler.dll ${bin}/dxil.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools DirectX12_Tools directx12toolconfig.cpp directx12toolconfig.h directx12toolsforward.h directx12toolslib.h)
		FolderSources(sources DirectX12_Tools imgui imgui_impl_dx12.cpp imgui_impl_dx12.h directx12imroot.cpp directx12imroot.h)


add_tools(DirectX12 ${sources} PRECOMPILED_HEADER tools/DirectX12_Tools/directx12toolslib.h)

target_link_plugins(DirectX12Tools ClientTools)
target_link_libraries(DirectX12Tools PRIVATE d3d12 dxgi dxcompiler)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()
