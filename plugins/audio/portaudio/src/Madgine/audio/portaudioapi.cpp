#include "../portaudioapilib.h"

#include "portaudioapi.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/root/root.h"

#include "Generic/execution/execution.h"

#include "portaudio.h"

#include "Madgine/resources/sender.h"

METATABLE_BEGIN_BASE(Engine::Audio::PortAudioApi, Engine::Audio::AudioApi)
METATABLE_END(Engine::Audio::PortAudioApi)

UNIQUECOMPONENT(Engine::Audio::PortAudioApi)

namespace Engine {
namespace Audio {

    struct PlaybackState : BehaviorReceiver {
        PlaybackState(AudioLoader::Handle buffer, PortAudioApi *api)
            : mBuffer(std::move(buffer))
            , mApi(api)
            , mStopCallback(this)
        {
        }

        void start();

        friend const void *tag_invoke(const Execution::get_debug_data_t &, PlaybackState &state)
        {
            return &state;
        }

        AudioLoader::Handle mBuffer;
        PortAudioApi *mApi;

        const void *mPtr;
        const void *mEnd;

        bool mLooping = false;

        struct stop_cb {
            stop_cb(PortAudioStream &stream)
                : mStream(stream)
            {
            }

            bool operator()();

            PortAudioStream &mStream;
        };

        struct finally_cb {
            finally_cb(PlaybackState *state)
                : mState(state)
            {
            }

            void operator()(Execution::cancelled_t)
            {
                mState->set_done();
            }
            void operator()()
            {
                mState->set_value();
            }

            PlaybackState *mState;
        };

        Execution::stop_callback<stop_cb, finally_cb> mStopCallback;
    };
     
    template <typename Rec>
    struct PlaybackStateImpl : VirtualBehaviorState<Rec, PlaybackState> {
        using VirtualBehaviorState<Rec, PlaybackState>::VirtualBehaviorState;
    };

    struct PlaybackSender : Execution::base_sender {
        using result_type = GenericResult;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, PlaybackSender &&sender, Rec &&rec)
        {
            return PlaybackStateImpl<Rec> { std::forward<Rec>(rec), std::move(sender.mBuffer), sender.mApi };
        }

        template <typename F>
        friend void tag_invoke(Execution::visit_state_t, PlaybackSender &sender, F &&f)
        {
            f(Execution::State::BeginBlock { "Play '"s + std::string { sender.mBuffer.name() } + "'" });

            f(Execution::State::Contextual {
                [](const void *context) -> Execution::StateDescriptor {
                    float progress = 0.0f;
                    if (context) {
                        const PlaybackState *state = static_cast<const PlaybackState *>(context);
                        const std::byte *start, *end, *current;
                        start = static_cast<const std::byte *>(state->mBuffer->mBuffer.begin());
                        end = static_cast<const std::byte *>(state->mEnd);
                        current = static_cast<const std::byte *>(state->mPtr);
                        progress = static_cast<float>(current - start) / (end - start);
                    }
                    return Execution::State::Progress { progress };
                } });

            f(Execution::State::EndBlock {});
        }

        static constexpr size_t debug_start_increment = 1;
        static constexpr size_t debug_operation_increment = 1;
        static constexpr size_t debug_stop_increment = 1;

        AudioLoader::Handle mBuffer;
        PortAudioApi *mApi;
    };

    struct PortAudioStream {

        PortAudioStream(const AudioInfo &info, PaDeviceIndex device, const PaDeviceInfo *deviceInfo)
            : mChannels(info.mChannels)
            , mSampleRate(info.mSampleRate)
        {
            PaStreamParameters outputParameters;
            outputParameters.channelCount = info.mChannels;
            outputParameters.sampleFormat = paInt16;
            outputParameters.hostApiSpecificStreamInfo = nullptr;
            outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
            outputParameters.device = device;

            PaError err = Pa_OpenStream(&mStream,
                nullptr,
                &outputParameters,
                /* info.mSampleRate*/ deviceInfo->defaultSampleRate,
                paFramesPerBufferUnspecified,
                paNoFlag,
                sCallback,
                this);
            if (err != paNoError)
                throw 0;

            err = Pa_SetStreamFinishedCallback(mStream, sFinishedCallback);
            if (err != paNoError)
                throw 0;
        }

        ~PortAudioStream()
        {
            PaError err = Pa_CloseStream(mStream);
            if (err != paNoError)
                throw 0;
            mStream = nullptr;
        }

        void play(PlaybackState &state)
        {
            assert(!mState);
            state.mPtr = state.mBuffer->mBuffer.begin();
            state.mEnd = state.mBuffer->mBuffer.end();
            mState = &state;
            abort();
            PaError err = Pa_StartStream(mStream);
            if (err != paNoError) {
                state.set_error(BEHAVIOR_UNKNOWN_ERROR() << "PortAudio Error: " << err);
                mState = nullptr;
                state.mApi->reuseStream(*this);
            }
        }

        bool abort()
        {
            if (!Pa_IsStreamStopped(mStream)) {
                PaError err = Pa_AbortStream(mStream);
                if (err != paNoError)
                    throw 0;
                return true;
            } else {
                return false;
            }
        }

        bool isCompatible(const AudioInfo &info) const
        {
            return mChannels == info.mChannels && mSampleRate == info.mSampleRate;
        }

    protected:
        int callback(
            void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo *timeInfo,
            PaStreamCallbackFlags statusFlags)
        {
            if (!Engine::Root::Root::getSingleton().taskQueue()->running())
                return paAbort;

            int16_t *target = static_cast<int16_t *>(output);
            const int16_t *source = static_cast<const int16_t *>(mState->mPtr);

            unsigned long count = std::min<unsigned int>(frameCount, (static_cast<const int16_t *>(mState->mEnd) - source) / mChannels);

            for (size_t i = 0; i < count; ++i) {
                for (int i = 0; i < mChannels; ++i)
                    *target++ = *source++;
            }

            mState->mPtr = source;
            if (mState->mPtr == mState->mEnd) {
                if (mState->mLooping) {
                    mState->mPtr = mState->mBuffer->mBuffer.begin();
                    return callback(target, frameCount - count, timeInfo, statusFlags);
                } else {
                    return paComplete;
                }
            } else {
                return paContinue;
            }
        }

        void finishedCallback()
        {
            PortAudioApi *api = static_cast<PortAudioApi *>(mState->mApi);
            mState->mStopCallback.finish();
            mState = nullptr;
            api->reuseStream(*this);
        }

        static int
        sCallback(const void *input,
            void *output,
            unsigned long frameCount,
            const PaStreamCallbackTimeInfo *timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData)
        {
            return static_cast<PortAudioStream *>(userData)->callback(output, frameCount, timeInfo, statusFlags);
        }

        static void sFinishedCallback(void *userData)
        {
            static_cast<PortAudioStream *>(userData)->finishedCallback();
        }

    private:
        PaStream *mStream = nullptr;
        PlaybackState *mState = nullptr;

        int mChannels;
        int mSampleRate;
    };

    void PlaybackState::start()
    {
        PortAudioStream &stream = mApi->fetchStream(mBuffer->mInfo);
        stream.play(*this);
        mStopCallback.start(stopToken(), stream);
    }

    bool PlaybackState::stop_cb::operator()()
    {
        return mStream.abort();
    }

    PortAudioApi::PortAudioApi(Root::Root &root)
        : AudioApiImpl<PortAudioApi>(root)
    {

        root.taskQueue()->addSetupSteps(
            [this]() {
                PaError err = Pa_Initialize();
                if (err != paNoError) {
                    LOG_ERROR("PortAudio error: " << Pa_GetErrorText(err));
                    return false;
                }

                PaHostApiIndex apiCount = Pa_GetHostApiCount();
                if (apiCount < 0) {
                    LOG_ERROR("PortAudio API count error: " << apiCount);
                    PaError err = Pa_Terminate();
                    if (err != paNoError)
                        LOG_ERROR("PortAudio error: " << Pa_GetErrorText(err));
                    return false;
                }

                PaDeviceIndex bestDeviceNum = -1;
                float bestDeviceLatency = 100.0f;

                for (PaHostApiIndex i = 0; i < apiCount; ++i) {
                    const PaHostApiInfo *apiInfo = Pa_GetHostApiInfo(i);
                    if (apiInfo->defaultOutputDevice >= 0) {
                        const PaDeviceInfo *info = Pa_GetDeviceInfo(apiInfo->defaultOutputDevice);
                        if (info->defaultSampleRate == 48000) {
                            if (info->defaultLowOutputLatency < bestDeviceLatency) {
                                bestDeviceLatency = info->defaultLowOutputLatency;
                                bestDeviceNum = apiInfo->defaultOutputDevice;
                            }
                        }
                    }
                }

                if (bestDeviceNum < 0) {
                    LOG_ERROR("PortAudio failed to find device!");
                    PaError err = Pa_Terminate();
                    if (err != paNoError)
                        LOG_ERROR("PortAudio error: " << Pa_GetErrorText(err));
                    return false;
                }

                mDevice = bestDeviceNum;
                mDeviceInfo = Pa_GetDeviceInfo(mDevice);

                return true;
            },
            [this](bool wasInitialized) {
                if (wasInitialized) {
                    std::vector<PortAudioStream *> streams;
                    std::ranges::transform(mBusyStreams, std::back_inserter(streams), [](auto &v) { return &v; });
                    for (PortAudioStream *stream : streams)
                        stream->abort();
                    assert(mBusyStreams.empty());
                    mStreamPool.clear();
                    PaError err = Pa_Terminate();
                    if (err != paNoError)
                        LOG_ERROR("PortAudio error: " << Pa_GetErrorText(err));
                }
            });
    }

    PortAudioApi::~PortAudioApi()
    {
    }

    std::string_view PortAudioApi::key() const
    {
        return "PortAudio";
    }

    Behavior PortAudioApi::playSound(AudioLoader::Handle buffer)
    {
        return PlaybackSender { {}, buffer, this } | Resources::with_handle(AudioLoader::Handle { buffer });
    }

    PortAudioStream &PortAudioApi::fetchStream(const AudioInfo &info)
    {
        std::unique_lock lock { mMutex };
        auto it = std::ranges::find_if(mStreamPool, [&](const PortAudioStream &stream) { return stream.isCompatible(info); });
        if (it == mStreamPool.end()) {
            return mBusyStreams.emplace_back(info, mDevice, mDeviceInfo);
        } else {
            mBusyStreams.splice(mBusyStreams.end(), mStreamPool, it);
            return *it;
        }
    }

    void PortAudioApi::reuseStream(PortAudioStream &stream)
    {
        std::unique_lock lock { mMutex };
        auto it = std::ranges::find(mBusyStreams, &stream, [](auto &v) { return &v; });
        mStreamPool.splice(mStreamPool.end(), mBusyStreams, it);
    }

}
}
