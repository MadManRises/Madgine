#include "../oboeapilib.h"

#include "oboeapi.h"

#include <oboe/Oboe.h>

#include "Generic/execution/execution.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/resources/sender.h"
#include "Madgine/root/root.h"

#include "Meta/reflect/metatable_impl.h"

METATABLE_BEGIN_BASE(Engine::Audio::OboeApi, Engine::Audio::AudioApi)
METATABLE_END(Engine::Audio::OboeApi)

UNIQUECOMPONENT(Engine::Audio::OboeApi)

namespace Engine {
namespace Audio {

    struct PlaybackState : Behavior::BehaviorReceiver {
        PlaybackState(const AudioBuffer &buffer, OboeApi &api, AudioOptions options, Execution::BindingPtr<Math::Vector2> distance)
            : mBuffer(buffer)
            , mApi(api)
            , mOptions(std::move(options))
            , mDistance(std::move(distance))
        {
        }

        void start();
        void stop();

        const AudioBuffer &mBuffer;
        OboeApi &mApi;
        AudioOptions mOptions;
        Execution::BindingPtr<Math::Vector2> mDistance;
        const void *mNext;
        const void *mEnd;
    };

    struct PlaybackSender : Execution::base_sender {
        using result_type = Reflect::Error;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, PlaybackSender &&sender, Rec &&rec)
        {
            return Behavior::VirtualBehaviorState<Rec, PlaybackState> { std::forward<Rec>(rec), sender.mBuffer, sender.mApi, std::move(sender.mOptions), std::move(sender.mDistance) };
        }

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, PlaybackSender &sender, Rec &&rec)
        {
            return Behavior::VirtualBehaviorState<Rec, PlaybackState> { std::forward<Rec>(rec), sender.mBuffer, sender.mApi, sender.mOptions, sender.mDistance };
        }

        const AudioBuffer &mBuffer;
        OboeApi &mApi;
        AudioOptions mOptions;
        Execution::BindingPtr<Math::Vector2> mDistance;
    };

    struct OboeErrorCallback : oboe::AudioStreamErrorCallback {
    };

    struct OboeStream : oboe::AudioStreamDataCallback, std::enable_shared_from_this<OboeStream> {

        void play(PlaybackState &state)
        {
            assert(!mState);
            state.mNext = state.mBuffer.mBuffer.begin();
            state.mEnd = state.mBuffer.mBuffer.end();
            mState = &state;
            mStream->requestStart();
        }

        bool isCompatible(const AudioInfo &info) const
        {
            return mStream->getChannelCount() == info.mChannels && mStream->getSampleRate() == info.mSampleRate;
        }

        oboe::DataCallbackResult onAudioReady(
            oboe::AudioStream *audioStream,
            void *audioData,
            int32_t numFrames) override
        {
            if (!Engine::Core::Root::getSingleton().taskQueue()->running()) {
                OboeApi &api = mState->mApi;
                mState->set_done();
                mState = nullptr;
                api.reuseStream(shared_from_this());
                return oboe::DataCallbackResult::Stop;
            }

            int count = AudioApi::process(audioData, audioStream->getFormat() == oboe::AudioFormat::I16 ? AudioApi::I16 : AudioApi::F, reinterpret_cast<const int16_t *&>(mState->mNext), mState->mEnd, numFrames, audioStream->getChannelCount(), mState->mDistance, mState->mOptions);
            
            if (count > 0) {
                return oboe::DataCallbackResult::Continue;
            } else {
                OboeApi &api = mState->mApi;
                mState->set_value();
                mState = nullptr;
                api.reuseStream(shared_from_this());

                return oboe::DataCallbackResult::Stop;
            }
        }

        std::shared_ptr<oboe::AudioStream> mStream;

    private:
        PlaybackState *mState = nullptr;
    };

    void PlaybackState::start()
    {
        std::shared_ptr<OboeStream> stream = mApi.fetchStream(mBuffer.mInfo);
        stream->play(*this);
    }

    void PlaybackState::stop()
    {
        throw 0;
    }

    OboeApi::OboeApi(Core::Root &root)
        : AudioApiImpl<OboeApi>(root)
    {
        mErrorCallback = std::make_shared<OboeErrorCallback>();
    }

    OboeApi::~OboeApi()
    {
    }

    std::string_view OboeApi::key() const
    {
        return "Oboe";
    }

    
    Behavior::Behavior OboeApi::playDynamicSound(AudioLoader::Handle buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options)
    {
        return PlaybackSender { {}, *buffer.getUnsafe(), *this, options, distance } | Resources::with_handle(AudioLoader::Handle { buffer });
    }

    Behavior::Behavior OboeApi::playDynamicSound(const AudioBuffer &buffer, Execution::BindingPtr<Math::Vector2> distance, const AudioOptions &options)
    {
        return PlaybackSender { {}, buffer, *this, options, distance };
    }

    std::shared_ptr<OboeStream> OboeApi::fetchStream(const AudioInfo &info)
    {
        std::unique_lock lock { mMutex };
        auto it = std::ranges::find_if(mStreamPool, [&](const std::shared_ptr<OboeStream> &stream) { return stream->isCompatible(info); });
        if (it == mStreamPool.end()) {
            std::shared_ptr<OboeStream> stream = std::make_shared<OboeStream>();
            oboe::AudioStreamBuilder builder;
            oboe::Result result = builder.setSharingMode(oboe::SharingMode::Exclusive)
                                      ->setPerformanceMode(oboe::PerformanceMode::LowLatency)
                                      ->setFormat(oboe::AudioFormat::I16)
                                      ->setChannelCount(info.mChannels)
                                      ->setSampleRate(info.mSampleRate)
                                      ->setDataCallback(stream)
                                      ->setErrorCallback(mErrorCallback)
                                      // Open using a shared_ptr.
                                      ->openStream(stream->mStream);
            return stream;
        } else {
            std::shared_ptr<OboeStream> stream = std::move(*it);
            mStreamPool.erase(it);
            return stream;
        }
    }

    void OboeApi::reuseStream(std::shared_ptr<OboeStream> stream)
    {
        mStreamPool.emplace_back(std::move(stream));
    }

}
}
