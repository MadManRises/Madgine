#include "directx11lib.h"

#include "directx11programloader.h"

#include "directx11vertexshaderloader.h"
#include "directx11pixelshaderloader.h"


#include "util/directx11program.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX11ProgramLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX11ProgramLoader, Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::DirectX11ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX11ProgramLoader::ResourceType, Engine::Render::ProgramLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX11ProgramLoader::ResourceType)



namespace Engine {
namespace Render {

    DirectX11ProgramLoader::DirectX11ProgramLoader()
    {
    }

    bool DirectX11ProgramLoader::loadImpl(DirectX11Program &program, ResourceDataInfo &info)
    {
        throw 0;
    }

    void DirectX11ProgramLoader::unloadImpl(DirectX11Program &program, ResourceDataInfo &info)
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

    bool DirectX11ProgramLoader::create(Program &_program, const std::string &name, const CodeGen::ShaderFile &file)
    {   
        assert(file.mInstances.size() == 2);

        DirectX11Program &program = static_cast<DirectX11Program &>(_program);

        DirectX11VertexShaderLoader::HandleType vertexShader;
        vertexShader.create(name, file);
        DirectX11PixelShaderLoader::HandleType pixelShader;
        pixelShader.create(name, file);

        if (!program.link(std::move(vertexShader), std::move(pixelShader)))
            std::terminate();

        return true;
    }

    void DirectX11ProgramLoader::setParameters(Program &program, const ByteBuffer &data, size_t index)
    {
        static_cast<DirectX11Program &>(program).setParameters(data, index);
    }

    WritableByteBuffer Engine::Render::DirectX11ProgramLoader::mapParameters(Program &program, size_t index)
    {
        return static_cast<DirectX11Program&>(program).mapParameters(index);
    }

	void DirectX11ProgramLoader::setDynamicParameters(Program &program, const ByteBuffer &data, size_t index)
    {
        static_cast<DirectX11Program &>(program).setDynamicParameters(data, index);
    }

}
}
