#pragma once

#include "Madgine/audio/audioapi.h"

typedef int PaDeviceIndex;
struct PaDeviceInfo;

namespace Engine {
namespace Audio {
    
    struct PortAudioApi : AudioApiImpl<PortAudioApi> {

        PortAudioApi(Root::Root &root);
        ~PortAudioApi();

        virtual std::string_view key() const override;

        virtual Behavior playSound(AudioLoader::Handle buffer) override;

    protected:
        friend struct PlaybackState;
        friend struct PortAudioStream;
        PortAudioStream &fetchStream(const AudioInfo &info);
        void reuseStream(PortAudioStream &stream);

    private:      
        std::mutex mMutex;
        std::list<PortAudioStream> mStreamPool;
        std::list<PortAudioStream> mBusyStreams;

        PaDeviceIndex mDevice;
        const PaDeviceInfo *mDeviceInfo;
    };

}
}

REGISTER_TYPE(Engine::Audio::PortAudioApi)