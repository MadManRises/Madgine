#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/threadstorage.h"

#include "audiostream.h"

namespace Engine {
namespace Audio {

    struct MADGINE_AUDIOLOADER_EXPORT AudioStreamLoader : Resources::ResourceLoader<AudioStreamLoader, AudioStream, std::list<Placeholder<0>>, Threading::ThreadStorage> {

        using Base = Resources::ResourceLoader<AudioStreamLoader, AudioStream, std::list<Placeholder<0>>, Threading::ThreadStorage>;

        AudioStreamLoader();

        bool loadImpl(AudioStream &data, ResourceType *res);
        void unloadImpl(AudioStream &data, ResourceType *res);
    };

}
}

RegisterType(Engine::Audio::AudioStreamLoader);