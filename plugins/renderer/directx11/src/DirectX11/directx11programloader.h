#pragma once

#include "Madgine/resources/resourceloader.h"

#include "programloader.h"

#include "util/directx11program.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11ProgramLoader : Resources::VirtualResourceLoaderImpl<DirectX11ProgramLoader, DirectX11Program, ProgramLoader> {
        DirectX11ProgramLoader();

        bool loadImpl(DirectX11Program &program, ResourceDataInfo &info);
        void unloadImpl(DirectX11Program &program, ResourceDataInfo &info);
        Threading::Task<bool> create(Program &program, const std::string &name, const std::vector<size_t> &bufferSizes = {}, size_t instanceDataSize = 0) override;
        bool create(Program &program, const std::string &name, const CodeGen::ShaderFile &file) override;

        virtual WritableByteBuffer mapParameters(Program &program, size_t index) override;
        
        virtual void setInstanceData(Program &program, const ByteBuffer &data) override;

        virtual void setDynamicParameters(Program &program, size_t index, const ByteBuffer &data) override;

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };
}
}

RegisterType(Engine::Render::DirectX11ProgramLoader);