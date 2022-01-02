#include "directx12lib.h"

#include "directx12programloader.h"

#include "directx12pixelshaderloader.h"
#include "directx12vertexshaderloader.h"

#include "util/directx12program.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

VIRTUALUNIQUECOMPONENT(Engine::Render::DirectX12ProgramLoader);

METATABLE_BEGIN_BASE(Engine::Render::DirectX12ProgramLoader, Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::DirectX12ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::DirectX12ProgramLoader::ResourceType, Engine::Render::ProgramLoader::ResourceType)
METATABLE_END(Engine::Render::DirectX12ProgramLoader::ResourceType)

namespace Engine {
namespace Render {

    DirectX12ProgramLoader::DirectX12ProgramLoader()
    {
    }

    bool DirectX12ProgramLoader::loadImpl(DirectX12Program &program, ResourceDataInfo &info)
    {
        throw 0;
    }

    void DirectX12ProgramLoader::unloadImpl(DirectX12Program &program, ResourceDataInfo &info)
    {
        program.reset();
    }

    Threading::Task<bool> DirectX12ProgramLoader::create(Program &_program, const std::string &name, const std::vector<size_t> &bufferSizes, size_t instanceDataSize)
    {
        DirectX12Program &program = static_cast<DirectX12Program &>(_program);

        DirectX12VertexShaderLoader::HandleType vertexShader;
        if (!co_await vertexShader.load(name))
            co_return false;
        DirectX12PixelShaderLoader::HandleType pixelShader;
        if (!co_await pixelShader.load(name))
            co_return false;

        if (!program.link(vertexShader, pixelShader))
            co_return false;

        for (size_t i = 0; i < bufferSizes.size(); ++i)
            if (bufferSizes[i] > 0)
                program.setParametersSize(i, bufferSizes[i]);

        if (instanceDataSize > 0)
            program.setInstanceDataSize(instanceDataSize);


        co_return true;
    }

    bool DirectX12ProgramLoader::create(Program &_program, const std::string &name, const CodeGen::ShaderFile &file)
    {
        assert(file.mInstances.size() == 2);

        DirectX12Program &program = static_cast<DirectX12Program &>(_program);

        DirectX12VertexShaderLoader::HandleType vertexShader;
        vertexShader.create(name, file);
        DirectX12PixelShaderLoader::HandleType pixelShader;
        pixelShader.create(name, file);

        if (!program.link(std::move(vertexShader), std::move(pixelShader)))
            std::terminate();

        return true;
    }

    WritableByteBuffer DirectX12ProgramLoader::mapParameters(Program &program, size_t index)
    {
        return static_cast<DirectX12Program &>(program).mapParameters(index);
    }

    void DirectX12ProgramLoader::setInstanceData(Program &program, const ByteBuffer &data)
    {
        static_cast<DirectX12Program &>(program).setInstanceData(data);
    }

    void DirectX12ProgramLoader::setDynamicParameters(Program &program, size_t index, const ByteBuffer &data)
    {
        static_cast<DirectX12Program &>(program).setDynamicParameters(index, data);
    }

}
}
