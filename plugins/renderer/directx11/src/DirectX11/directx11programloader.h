#pragma once

#include "Modules/resources/resourceloader.h"

#include "programloader.h"

#include "util/directx11program.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11ProgramLoader : Resources::VirtualResourceLoaderImpl<DirectX11ProgramLoader, DirectX11Program, ProgramLoader> {
        DirectX11ProgramLoader();

        bool loadImpl(DirectX11Program &program, ResourceType *res);
        void unloadImpl(DirectX11Program &program, ResourceType *res);
        bool create(Program &program, const std::string &name) override;

        virtual void setParameters(Program &program, const void *data, size_t size, size_t index) override;
    };
}
}