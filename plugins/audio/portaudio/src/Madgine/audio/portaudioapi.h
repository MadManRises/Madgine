#pragma once

#include "Modules/threading/madgineobject.h"

#include "Madgine/audio/audioapi.h"

typedef int PaDeviceIndex;
struct PaDeviceInfo;

namespace Engine {
namespace Audio {

    struct PortAudioApi : AudioApiImpl<PortAudioApi>, Threading::MadgineObject<PortAudioApi> {

        PortAudioApi(Core::Root &root);
        ~PortAudioApi();

        bool init();
        void finalize();

        std::string_view key() const override;

        Behavior::Behavior playDynamicSound(AudioLoader::Handle buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options) override;        
        Behavior::Behavior playDynamicSound(const AudioBuffer &buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options) override;

    protected:
        friend struct PlaybackState;
        friend struct PortAudioStream;
        PortAudioStream &fetchStream(const AudioInfo &info);
        void reuseStream(PortAudioStream &stream);

    private:
        std::mutex mMutex;
        std::list<PortAudioStream> mStreamPool;
        std::list<PortAudioStream> mBusyStreams;
        std::list<PortAudioStream> mClosingStreams;

        PaDeviceIndex mDevice;
        const PaDeviceInfo *mDeviceInfo = nullptr;
    };

}
}