#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/threadstorage.h"

#include "audiostream.h"

namespace Engine {
namespace Audio {

    struct MADGINE_AUDIOLOADER_EXPORT AudioStreamLoader : Resources::ResourceLoader<AudioStreamLoader, AudioStream, std::list<Placeholder<0>>, Threading::ThreadStorage> {

        using Base = Resources::ResourceLoader<AudioStreamLoader, AudioStream, std::list<Placeholder<0>>, Threading::ThreadStorage>;

        AudioStreamLoader();

        bool loadImpl(AudioStream &data, ResourceDataInfo &info);
        void unloadImpl(AudioStream &data, ResourceDataInfo &info);
    };

}
}

RegisterType(Engine::Audio::AudioStreamLoader);