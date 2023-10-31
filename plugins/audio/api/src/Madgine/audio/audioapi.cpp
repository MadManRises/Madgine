#include "../audiolib.h"

#include "audioapi.h"

#include "Madgine/root/root.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Generic/execution/execution.h"

METATABLE_BEGIN(Engine::Audio::AudioApi)
METATABLE_END(Engine::Audio::AudioApi)

VIRTUALUNIQUECOMPONENTBASE(Engine::Audio::AudioApi)

namespace Engine {
namespace Audio {

    AudioApi::AudioApi(Root::Root &root)
        : VirtualRootComponentBase(root)
    {
    }

    void AudioApi::playSound(std::string_view name)
    {
        playSound(AudioLoader::load(name));
    }

    void AudioApi::playSound(AudioLoader::Handle buffer)
    {
        Execution::detach(playSoundSender(std::move(buffer)));
    }

    AudioApi::PlaybackState::PlaybackState(AudioApi *api, AudioLoader::Handle buffer)
        : mApi(api)
        , mBuffer(std::move(buffer))
    {
    }

    void AudioApi::PlaybackState::start()
    {
        mBuffer.info()->loadingTask().then([this](bool success) {
            if (success)
                mApi->playSoundImpl(*this);
            else
                set_error(AudioResult::UNKNOWN_ERROR);
        },
            Root::Root::getSingleton().taskQueue());
    }

}
}
