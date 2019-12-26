#include "opengllib.h"

#include "openglprogramloader.h"

#include "openglshaderloader.h"

#include "util/openglshader.h"

#include "util/openglprogram.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLProgramLoader);

METATABLE_BEGIN_BASE(Engine::Render::OpenGLProgramLoader, Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::OpenGLProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::OpenGLProgramLoader::ResourceType, Engine::Render::ProgramLoader::ResourceType)
METATABLE_END(Engine::Render::OpenGLProgramLoader::ResourceType)

RegisterType(Engine::Render::OpenGLProgramLoader);

namespace Engine {
namespace Render {

    OpenGLProgramLoader::OpenGLProgramLoader()
    {
    }

    bool OpenGLProgramLoader::loadImpl(OpenGLProgram &program, ResourceType *res)
    {
        throw 0;
    }

    void OpenGLProgramLoader::unloadImpl(OpenGLProgram &program, ResourceType *res)
    {
        program.reset();
    }

    bool OpenGLProgramLoader::create(Program &_program, const std::string &name)
    {
        OpenGLProgram &program = static_cast<OpenGLProgram &>(_program);

        OpenGLShaderLoader::HandleType vertexShader;
        vertexShader.load(name + "_VS");
        OpenGLShaderLoader::HandleType pixelShader;
        pixelShader.load(name + "_PS");

        if (!program.link(vertexShader, pixelShader, { "aPos", "aPos2", "aColor", "aNormal", "aUV" }))
            std::terminate();

        return true;
    }

    void OpenGLProgramLoader::bind(Program &program)
    {
        static_cast<OpenGLProgram &>(program).bind();
    }

    void OpenGLProgramLoader::setUniform(Program &program, const std::string &var, int value)
    {
        static_cast<OpenGLProgram &>(program).setUniform(var, value);
    }

    void OpenGLProgramLoader::setUniform(Program &program, const std::string &var,
        const Matrix3 &value)
    {
        static_cast<OpenGLProgram &>(program).setUniform(var, value);
    }

    void OpenGLProgramLoader::setUniform(Program &program, const std::string &var, const Matrix4 &value)
    {
        static_cast<OpenGLProgram &>(program).setUniform(var, value);
    }

    void OpenGLProgramLoader::setUniform(Program &program, const std::string &var, const Vector3 &value)
    {
        static_cast<OpenGLProgram &>(program).setUniform(var, value);
    }

}
}
