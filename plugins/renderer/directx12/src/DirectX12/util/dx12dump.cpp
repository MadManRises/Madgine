#include "../directx12lib.h"

#include "../directx12rendercontext.h"

using namespace Engine::Render;

bool checkDevice(HRESULT& result) {
    if (!GetDevice())
        return true;
    HRESULT reason = GetDevice()->GetDeviceRemovedReason();
    if (FAILED(reason)) {
        result = DXGI_ERROR_DEVICE_REMOVED;
        return false;
    }
    return true;
}

void dx12Dump(HRESULT result)
{

    Engine::Log::LogDummy cout { Engine::Log::MessageType::INFO_TYPE };

    
    if (result == DXGI_ERROR_DEVICE_REMOVED) {
        HRESULT reason = GetDevice()->GetDeviceRemovedReason();
        int i = 3;
    }

    cout << "DX12-State: ---------- \n";

    /*GLenum glenum;
    GLint glint;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &glint);
    cout << "GL_ARRAY_BUFFER: " << glint << "\n";

    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &glint);
    cout << "GL_ELEMENT_ARRAY_BUFFER: " << glint << "\n";

    cout << "GL_VERTEX_ARRAY: " << DirectX12VertexArray::getCurrent() << "\n";
	#if DIRECTX12_ES
    auto[vbo, ebo] = DirectX12VertexArray::getCurrentBindings();
    cout << "  binding: GL_ARRAY_BUFFER: " << vbo << ", GL_ELEMENT_ARRAY_BUFFER: " << ebo << "\n";
	#endif*/

    cout << "DX12-State End ---------";
}