#pragma once

#include "Madgine/resources/resourceloader.h"

#include "audiobuffer.h"

namespace Engine {
namespace Audio {

    struct AudioBuffer;

    struct MADGINE_AUDIOLOADER_EXPORT AudioBufferLoader : Resources::ResourceLoader<AudioBufferLoader, AudioBuffer> {

        using Base = Resources::ResourceLoader<AudioBufferLoader, AudioBuffer>;

        AudioBufferLoader();

        bool loadImpl(AudioBuffer &data, ResourceDataInfo &info);
        void unloadImpl(AudioBuffer &data);
    };

}
}

REGISTER_TYPE(Engine::Audio::AudioBufferLoader)