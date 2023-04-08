Project(DirectX11)
	


set(sources "")
FolderSources(sources "" src)
FolderSources(sources src DirectX11 directx11forward.h directx11geometryshaderloader.cpp directx11geometryshaderloader.h directx11lib.h directx11meshdata.h directx11meshloader.cpp directx11meshloader.h directx11pixelshaderloader.cpp directx11pixelshaderloader.h directx11pipelineloader.cpp directx11pipelineloader.h directx11rendertexture.cpp directx11rendertexture.h directx11renderwindow.cpp directx11renderwindow.h directx11vertexshaderloader.cpp directx11vertexshaderloader.h directx11textureloader.cpp directx11textureloader.h directx11rendertarget.cpp directx11rendertarget.h directx11rendercontext.cpp directx11rendercontext.h directx11shadercodegen.cpp directx11shadercodegen.h)
	FolderSources(sources DirectX11 util dx11dump.cpp directx11buffer.cpp directx11buffer.h directx11texture.cpp directx11texture.h directx11pipelineinstance.cpp directx11pipelineinstance.h)

add_plugin(DirectX11 Madgine Renderer ${sources} PRECOMPILED_HEADER src/DirectX11/directx11lib.h)

target_link_libraries(DirectX11 PRIVATE d3d11 dxgi d3dcompiler Client)

set(sources "")
FolderSources(sources "" tools)
	FolderSources(sources tools DirectX11_Tools directx11toolconfig.cpp directx11toolconfig.h directx11toolsforward.h directx11toolslib.h)
		FolderSources(sources DirectX11_Tools imgui imgui_impl_dx11.cpp imgui_impl_dx11.h directx11imroot.cpp directx11imroot.h)


add_tools(DirectX11 ${sources} PRECOMPILED_HEADER tools/DirectX11_Tools/directx11toolslib.h)

target_link_plugins(DirectX11Tools ClientTools)
target_link_libraries(DirectX11Tools PRIVATE d3d11 dxgi d3dcompiler)

add_shadergen_target(DirectX11 -HLSL)
