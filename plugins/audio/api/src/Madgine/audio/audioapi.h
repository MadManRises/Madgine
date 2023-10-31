#pragma once

#include "Madgine/root/rootcomponentcollector.h"

#include "Madgine/root/rootcomponentbase.h"

#include "Generic/execution/virtualsender.h"

#include "Madgine/audioloader/audioloader.h"

namespace Engine {
namespace Audio {

    ENUM_BASE(AudioResult, GenericResult);

    struct MADGINE_AUDIO_EXPORT AudioApi : Root::VirtualRootComponentBase<AudioApi> {

        AudioApi(Root::Root &root);
    
        auto playSoundSender(AudioLoader::Handle buffer)
        {
            return Execution::make_virtual_sender<PlaybackState, AudioResult>(this, std::move(buffer));
        }

        void playSound(std::string_view name);
        void playSound(AudioLoader::Handle buffer);

    protected:
        struct LinkerDummy : Execution::VirtualReceiverBase<AudioResult> {
        };

        struct MADGINE_AUDIO_EXPORT PlaybackState : LinkerDummy {
            PlaybackState(AudioApi *api, AudioLoader::Handle buffer);

            void start();

            AudioApi *mApi;
            AudioLoader::Handle mBuffer;
        };

        virtual void playSoundImpl(PlaybackState &state) = 0;
    };

    template <typename T>
    using AudioApiImpl = VirtualScope<T, Root::RootComponentVirtualImpl<T, AudioApi>>;
}
}

REGISTER_TYPE(Engine::Audio::AudioApi)