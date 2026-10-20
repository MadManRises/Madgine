#pragma once

#include "Madgine/audio/audioapi.h"

namespace Engine {
namespace Audio {

    struct OboeErrorCallback;
    struct OboeStream;

    struct OboeApi : AudioApiImpl<OboeApi> {

        OboeApi(Core::Root &root);
        ~OboeApi();

        std::string_view key() const override;
        
        Behavior::Behavior playDynamicSound(AudioLoader::Handle buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options) override;
        Behavior::Behavior playDynamicSound(const AudioBuffer &buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options) override;

    protected:
        friend struct PlaybackState;
        friend struct OboeStream;
        std::shared_ptr<OboeStream> fetchStream(const AudioInfo &info);
        void reuseStream(std::shared_ptr<OboeStream> stream);

    private:
        std::shared_ptr<OboeErrorCallback> mErrorCallback;

        std::mutex mMutex;
        std::list<std::shared_ptr<OboeStream>> mStreamPool;
    };

}
}