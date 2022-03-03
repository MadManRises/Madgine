#include "directx11lib.h"

#include "directx11programloader.h"

#include "directx11geometryshaderloader.h"
#include "directx11pixelshaderloader.h"
#include "directx11vertexshaderloader.h"

#include "util/directx11program.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "codegen/codegen_shader.h"

#include "directx11rendercontext.h"

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

    void DirectX11ProgramLoader::unloadImpl(DirectX11Program &program)
    {
        program.reset();
    }

    Threading::Task<bool> DirectX11ProgramLoader::create(Program &_program, std::string name, std::vector<size_t> bufferSizes, size_t instanceDataSize)
    {
        DirectX11Program &program = static_cast<DirectX11Program &>(_program);

        DirectX11VertexShaderLoader::HandleType vertexShader;
        if (!co_await vertexShader.load(name))
            co_return false;
        DirectX11GeometryShaderLoader::HandleType geometryShader;
        if (!co_await geometryShader.load(name) && geometryShader)
            co_return false;
        DirectX11PixelShaderLoader::HandleType pixelShader;
        if (!co_await pixelShader.load(name) && pixelShader)
            co_return false;

        if (!program.link(std::move(vertexShader), std::move(pixelShader), std::move(geometryShader)))
            co_return false;

        for (size_t i = 0; i < bufferSizes.size(); ++i)
            if (bufferSizes[i] > 0)
                program.setParametersSize(i, bufferSizes[i]);

        if (instanceDataSize > 0)
            program.setInstanceDataSize(instanceDataSize);

        co_return true;
    }

    bool DirectX11ProgramLoader::create(Program &_program, const std::string &name, const CodeGen::ShaderFile &file)
    {
        assert(file.mInstances.size() == 2);

        DirectX11Program &program = static_cast<DirectX11Program &>(_program);

        DirectX11VertexShaderLoader::HandleType vertexShader;
        vertexShader.create(name, file);
        DirectX11GeometryShaderLoader::HandleType geometryShader;
        geometryShader.create(name, file);
        DirectX11PixelShaderLoader::HandleType pixelShader;
        pixelShader.create(name, file);

        if (!program.link(std::move(vertexShader), std::move(pixelShader), std::move(geometryShader)))
            std::terminate();

        return true;
    }

    WritableByteBuffer Engine::Render::DirectX11ProgramLoader::mapParameters(Program &program, size_t index)
    {
        return static_cast<DirectX11Program &>(program).mapParameters(index);
    }

    void DirectX11ProgramLoader::setInstanceData(Program &program, const ByteBuffer &data)
    {
        static_cast<DirectX11Program &>(program).setInstanceData(data);
    }

    void DirectX11ProgramLoader::setDynamicParameters(Program &program, size_t index, const ByteBuffer &data)
    {
        static_cast<DirectX11Program &>(program).setDynamicParameters(index, data);
    }

    Threading::TaskQueue *DirectX11ProgramLoader::loadingTaskQueue() const
    {
        return DirectX11RenderContext::renderQueue();
    }

}
}
