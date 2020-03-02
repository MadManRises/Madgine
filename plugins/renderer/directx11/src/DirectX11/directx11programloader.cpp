#include "directx11lib.h"

#include "directx11programloader.h"

#include "directx11vertexshaderloader.h"
#include "directx11pixelshaderloader.h"


#include "util/directx11program.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX11ProgramLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX11ProgramLoader, Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::DirectX11ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11ProgramLoader::ResourceType, Engine::Render::ProgramLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX11ProgramLoader::ResourceType)

RegisterType(Engine::Render::DirectX11ProgramLoader);

namespace Engine {
namespace Render {

    DirectX11ProgramLoader::DirectX11ProgramLoader()
    {
    }

    bool DirectX11ProgramLoader::loadImpl(DirectX11Program &program, ResourceType *res)
    {
        throw 0;
    }

    void DirectX11ProgramLoader::unloadImpl(DirectX11Program &program, ResourceType *res)
    {
        program.reset();
    }

    bool DirectX11ProgramLoader::create(Program &_program, const std::string &name)
    {
        DirectX11Program &program = static_cast<DirectX11Program &>(_program);

        DirectX11VertexShaderLoader::HandleType vertexShader;
        vertexShader.load(name);
        DirectX11PixelShaderLoader::HandleType pixelShader;
        pixelShader.load(name);

        if (!program.link(vertexShader, pixelShader))
            std::terminate();

        return true;
    }

    void DirectX11ProgramLoader::setParameters(Program &program, const ByteBuffer &data, size_t index)
    {
        static_cast<DirectX11Program &>(program).setParameters(data, index);
    }

	void DirectX11ProgramLoader::setDynamicParameters(Program &program, const ByteBuffer &data, size_t index)
    {
        static_cast<DirectX11Program &>(program).setDynamicParameters(data, index);
    }

}
}
