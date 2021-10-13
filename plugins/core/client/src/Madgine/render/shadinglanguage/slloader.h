#pragma once

#include "Madgine/resources/resourceloader.h"


namespace Engine {
namespace Render {

    struct SlFile {
    };

    struct SlLoader : Resources::ResourceLoader<SlLoader, SlFile> {
        SlLoader();

        bool loadImpl(SlFile &shader, ResourceDataInfo &info);
        void unloadImpl(SlFile &shader, ResourceDataInfo &info);
    };

}
}

RegisterType(Engine::Render::SlLoader);