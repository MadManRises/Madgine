#pragma once

#include "Madgine/resources/resourceloader.h"

#include "programloader.h"

#include "util/directx12program.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12ProgramLoader : Resources::VirtualResourceLoaderImpl<DirectX12ProgramLoader, DirectX12Program, ProgramLoader> {
        DirectX12ProgramLoader();

        bool loadImpl(DirectX12Program &program, ResourceDataInfo &info);
        void unloadImpl(DirectX12Program &program, ResourceDataInfo &info);
        bool create(Program &program, const std::string &name, const std::vector<size_t> &bufferSizes = {}, size_t instanceDataSize = 0) override;
        bool create(Program &program, const std::string &name, const CodeGen::ShaderFile &file) override;

        virtual WritableByteBuffer mapParameters(Program &program, size_t index) override;

        virtual void setInstanceData(Program &program, const ByteBuffer &data) override;
        
        virtual void setDynamicParameters(Program &program, size_t index, const ByteBuffer &data) override;
    };
}
}

RegisterType(Engine::Render::DirectX12ProgramLoader);