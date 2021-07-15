cmake_minimum_required (VERSION 2.6)

Project(DirectX12)
	


set(sources "")
FolderSources(sources "" src)
FolderSources(sources src DirectX12 directx12forward.h directx12lib.h directx12meshdata.h directx12meshloader.cpp directx12meshloader.h directx12pixelshaderloader.cpp directx12pixelshaderloader.h directx12programloader.cpp directx12programloader.h directx12rendertexture.cpp directx12rendertexture.h directx12renderwindow.cpp directx12renderwindow.h directx12vertexshaderloader.cpp directx12vertexshaderloader.h directx12textureloader.cpp directx12textureloader.h directx12rendertarget.cpp directx12rendertarget.h directx12rendercontext.cpp directx12rendercontext.h directx12shadercodegen.cpp directx12shadercodegen.h)
	FolderSources(sources DirectX12 util dx12dump.cpp directx12buffer.cpp directx12buffer.h directx12constantbufferheap.cpp directx12constantbufferheap.h directx12descriptorheap.cpp directx12descriptorheap.h directx12pixelshader.cpp directx12pixelshader.h directx12texture.cpp directx12texture.h directx12vertexarray.cpp directx12vertexarray.h directx12program.cpp directx12program.h directx12vertexshader.cpp directx12vertexshader.h)

add_plugin(DirectX12 Madgine Renderer ${sources} PRECOMPILED_HEADER src/DirectX12/directx12lib.h)

target_link_libraries(DirectX12 PRIVATE d3d12 dxgi dxguid d3dcompiler Client)


target_link_plugins(DirectX12 MeshLoader ProgramLoader TextureLoader ImageLoader)

set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools DirectX12_Tools directx12toolconfig.cpp directx12toolconfig.h directx12toolsforward.h directx12toolslib.h)
		FolderSources(sources DirectX12_Tools imgui imgui_impl_dx12.cpp imgui_impl_dx12.h directx12imroot.cpp directx12imroot.h)


add_tools(DirectX12 ${sources} PRECOMPILED_HEADER tools/DirectX12_Tools/directx12toolslib.h)

target_link_plugins(DirectX12Tools ClientTools FontLoader)
target_link_libraries(DirectX12Tools PRIVATE d3d12 dxgi d3dcompiler)

if (BUILD_TESTING)
	add_subdirectory(test)
endif()
