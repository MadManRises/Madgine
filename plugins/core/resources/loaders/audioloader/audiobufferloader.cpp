#include "audioloaderlib.h"

#include "audiobufferloader.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "oggdecode.h"

#include "Generic/stream.h"

UNIQUECOMPONENT(Engine::Audio::AudioBufferLoader)

METATABLE_BEGIN(Engine::Audio::AudioBufferLoader)
MEMBER(mResources)
METATABLE_END(Engine::Audio::AudioBufferLoader)

METATABLE_BEGIN_BASE(Engine::Audio::AudioBufferLoader::Resource, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Audio::AudioBufferLoader::Resource)

namespace Engine {
namespace Audio {

    AudioBufferLoader::AudioBufferLoader()
        : ResourceLoader(std::vector<std::string> { ".ogg" })
    {
    }

    bool AudioBufferLoader::loadImpl(AudioBuffer &data, ResourceDataInfo &info)
    {
        Stream stream = DecodeOggFile(data.mInfo, info.resource()->readAsStream(true));

        size_t bufferSize = (data.mInfo.mBitsPerSample * data.mInfo.mSampleCount * data.mInfo.mChannels) / 8;

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);

        size_t actualSize = stream.read(buffer.get(), bufferSize);
        if (actualSize != bufferSize)
            LOG_WARNING("Mismatch loading audio-file: " << info.resource()->path() << " Diff: " << bufferSize - actualSize);

        data.mBuffer = { std::move(buffer), actualSize };

        return true;
    }

    void AudioBufferLoader::unloadImpl(AudioBuffer &data)
    {
    }

}
}