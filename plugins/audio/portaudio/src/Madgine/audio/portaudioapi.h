#pragma once

#include "Madgine/audio/audioapi.h"

#include "Modules/threading/taskqueue.h"

namespace Engine {
namespace Audio {

    struct PortAudioApi : AudioApiImpl<PortAudioApi> {

        PortAudioApi(Root::Root &root);
        ~PortAudioApi();

        virtual std::string_view key() const override;

        virtual void playSoundImpl(PlaybackState &state) override;

    protected:
        struct PortAudioStream;
        PortAudioStream &fetchStream(const AudioInfo &info);
        void reuseStream(PortAudioStream &stream);

    private:      
        std::mutex mMutex;
        std::list<PortAudioStream> mStreamPool;
        std::list<PortAudioStream> mBusyStreams;
    };

}
}

REGISTER_TYPE(Engine::Audio::PortAudioApi)