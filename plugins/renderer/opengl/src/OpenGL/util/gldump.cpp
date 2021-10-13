#include "../opengllib.h"

#include "openglvertexarray.h"

using namespace Engine::Render;

void glDump()
{

    Engine::Util::LogDummy cout { Engine::Util::MessageType::LOG_TYPE };

    cout << "GL-State: ---------- \n";

    GLenum glenum;
    GLint glint;

    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &glint);
    cout << "GL_ARRAY_BUFFER: " << glint << "\n";

    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &glint);
    cout << "GL_ELEMENT_ARRAY_BUFFER: " << glint << "\n";

    //cout << "GL_VERTEX_ARRAY: " << OpenGLVertexArray::getCurrent() << "\n";
	#if OPENGL_ES
    auto[vbo, ebo] = OpenGLVertexArray::getCurrentBindings();
    cout << "  binding: GL_ARRAY_BUFFER: " << vbo << ", GL_ELEMENT_ARRAY_BUFFER: " << ebo << "\n";
	#endif

    cout << "GL-State End ---------";
}