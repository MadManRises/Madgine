#include "audioloaderlib.h"

#include "audiostreamloader.h"

#include "Modules/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::Audio::AudioStreamLoader)

METATABLE_BEGIN(Engine::Audio::AudioStreamLoader)
MEMBER(mResources)
METATABLE_END(Engine::Audio::AudioStreamLoader)

METATABLE_BEGIN_BASE(Engine::Audio::AudioStreamLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Audio::AudioStreamLoader::ResourceType)

namespace Engine {
namespace Audio {

    AudioStreamLoader::AudioStreamLoader()
        : ResourceLoader(std::vector<std::string> {})
    {
    }

    bool AudioStreamLoader::loadImpl(AudioStream &data, ResourceType *res)
    {
        return true;
    }

    void AudioStreamLoader::unloadImpl(AudioStream &data, ResourceType *res)
    {
    }

}
}