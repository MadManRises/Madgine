#pragma once

#include "Madgine/resources/resourceloader.h"


namespace Engine {
namespace Render {

    struct SlFile {
    };

    struct SlLoader : Resources::ResourceLoader<SlLoader, SlFile> {
        SlLoader();

        bool loadImpl(SlFile &shader, ResourceDataInfo &info);
        void unloadImpl(SlFile &shader);
    };

}
}

REGISTER_TYPE(Engine::Render::SlLoader)