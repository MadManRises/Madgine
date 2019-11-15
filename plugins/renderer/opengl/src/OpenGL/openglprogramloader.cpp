#include "opengllib.h"

#include "openglprogramloader.h"

#include "openglshaderloader.h"

#include "util/openglprogram.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::OpenGLProgramLoader);

using LoaderImpl = Engine::Resources::ResourceLoaderImpl<Engine::Render::Program, Engine::Resources::ThreadLocalResource>;

METATABLE_BEGIN_BASE(Engine::Render::OpenGLProgramLoader, LoaderImpl)
METATABLE_END(Engine::Render::OpenGLProgramLoader)

RegisterType(Engine::Render::OpenGLProgramLoader);

namespace Engine {
namespace Render {

    OpenGLProgramLoader::OpenGLProgramLoader()
    {
    }

    std::shared_ptr<Program> OpenGLProgramLoader::loadImpl(ResourceType *res)
    {
        throw 0;
    }

    std::shared_ptr<Program> OpenGLProgramLoader::create(const std::string &name)
    {
        std::shared_ptr<OpenGLProgram> program = std::make_shared<OpenGLProgram>();

        std::shared_ptr<OpenGLShader> vertexShader = OpenGLShaderLoader::load(name + "_VS");
        std::shared_ptr<OpenGLShader> pixelShader = OpenGLShaderLoader::load(name + "_PS");

        if (!program->link(vertexShader.get(), pixelShader.get(), { "aPos", "aPos2", "aColor", "aNormal", "aUV" }))
            std::terminate();

        return program;
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
