#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "audiobuffer.h"

namespace Engine {
namespace Audio {

    struct AudioBuffer;

    struct MADGINE_AUDIOLOADER_EXPORT AudioBufferLoader : Resources::ResourceLoader<AudioBufferLoader, AudioBuffer> {

        using Base = Resources::ResourceLoader<AudioBufferLoader, AudioBuffer>;

        AudioBufferLoader();

        bool loadImpl(AudioBuffer &data, ResourceType *res);
        void unloadImpl(AudioBuffer &data, ResourceType *res);
    };

}
}

RegisterType(Engine::Audio::AudioBufferLoader);