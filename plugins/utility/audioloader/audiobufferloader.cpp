#include "audioloaderlib.h"

#include "audiobufferloader.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "oggdecode.h"

#include "Interfaces/filesystem/api.h"
#include "Interfaces/streams/streams.h"

UNIQUECOMPONENT(Engine::Audio::AudioBufferLoader)

METATABLE_BEGIN(Engine::Audio::AudioBufferLoader)
MEMBER(mResources)
METATABLE_END(Engine::Audio::AudioBufferLoader)

METATABLE_BEGIN_BASE(Engine::Audio::AudioBufferLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Audio::AudioBufferLoader::ResourceType)

namespace Engine {
namespace Audio {

    AudioBufferLoader::AudioBufferLoader()
        : ResourceLoader(std::vector<std::string> { ".ogg" })
    {
    }

    bool AudioBufferLoader::loadImpl(AudioBuffer &data, ResourceType *res)
    {
        InStream stream = DecodeOggFile(data.mInfo, Filesystem::openFileRead(res->path(), true));

        size_t bufferSize = (data.mInfo.mBitsPerSample * data.mInfo.mSampleCount * data.mInfo.mChannels) / 8;

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(bufferSize);

        size_t actualSize = stream.readRaw(buffer.get(), bufferSize);
        if (actualSize != bufferSize)
            LOG_WARNING("Mismatch loading audio-file: " << res->path() << " Diff: " << bufferSize - actualSize);

        data.mBuffer = { std::move(buffer), actualSize };

        return true;
    }

    void AudioBufferLoader::unloadImpl(AudioBuffer &data, ResourceType *res)
    {
    }

}
}