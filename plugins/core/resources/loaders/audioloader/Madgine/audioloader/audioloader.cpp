#include "../audioloaderlib.h"

#include "audioloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "oggdecode.h"

#include "Generic/stream.h"

UNIQUECOMPONENT(Engine::Audio::AudioLoader)

METATABLE_BEGIN(Engine::Audio::AudioLoader)
MEMBER(mResources)
METATABLE_END(Engine::Audio::AudioLoader)

METATABLE_BEGIN_BASE(Engine::Audio::AudioLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Audio::AudioLoader::Resource)

namespace Engine {
namespace Audio {

    AudioLoader::AudioLoader()
        : ResourceLoader(std::vector<std::string> { ".ogg" })
    {
    }

    Stream AudioLoader::Interface::Resource::readAsStream(AudioInfo &info)
    {
        return DecodeOggFile(info, ResourceLoader::Interface::Resource::readAsStream(true));
    }

    bool AudioLoader::loadImpl(AudioBuffer &data, ResourceDataInfo &info)
    {
        Stream stream = info.resource()->readAsStream(data.mInfo);
        if (!stream)
            return false;

        size_t bufferSize = (data.mInfo.mBitsPerSample * data.mInfo.mSampleCount * data.mInfo.mChannels) / 8;

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);

        size_t actualSize = stream.read(buffer.get(), bufferSize);
        if (actualSize != bufferSize)
            LOG_WARNING("Mismatch loading audio-file: " << info.resource()->path() << " Diff: " << bufferSize - actualSize);

        data.mBuffer = { std::move(buffer), actualSize };

        return true;
    }

    void AudioLoader::unloadImpl(AudioBuffer &data)
    {
    }

}
}
