#include "../directx11lib.h"


using namespace Engine::Render;

void dx11Dump()
{

    Engine::Util::LogDummy cout { Engine::Util::MessageType::INFO_TYPE };

    cout << "DX11-State: ---------- \n";

    /*GLenum glenum;
    GLint glint;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &glint);
    cout << "GL_ARRAY_BUFFER: " << glint << "\n";

    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &glint);
    cout << "GL_ELEMENT_ARRAY_BUFFER: " << glint << "\n";

    cout << "GL_VERTEX_ARRAY: " << DirectX11VertexArray::getCurrent() << "\n";
	#if DIRECTX11_ES
    auto[vbo, ebo] = DirectX11VertexArray::getCurrentBindings();
    cout << "  binding: GL_ARRAY_BUFFER: " << vbo << ", GL_ELEMENT_ARRAY_BUFFER: " << ebo << "\n";
	#endif*/

    cout << "DX11-State End ---------";
}