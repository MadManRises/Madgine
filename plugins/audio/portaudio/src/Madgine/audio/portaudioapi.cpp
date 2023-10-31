#include "../portaudioapilib.h"

#include "portaudioapi.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/root/root.h"

#include "Generic/execution/execution.h"

#include "portaudio.h"

#include "Modules/threading/awaitables/awaitablesender.h"

METATABLE_BEGIN_BASE(Engine::Audio::PortAudioApi, Engine::Audio::AudioApi)
METATABLE_END(Engine::Audio::PortAudioApi)

UNIQUECOMPONENT(Engine::Audio::PortAudioApi)

namespace Engine {
namespace Audio {

    struct PortAudioApi::PortAudioStream {

        PortAudioStream(const AudioInfo &info)
            : mChannels(info.mChannels)
            , mSampleRate(info.mSampleRate)
        {
            PaError err = Pa_OpenDefaultStream(&mStream,
                0,
                info.mChannels,
                paInt16,
                info.mSampleRate,
                paFramesPerBufferUnspecified,
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
            mState = &state;
            mBuffer = state.mBuffer->mBuffer.begin();
            mEnd = state.mBuffer->mBuffer.end();
            abort();
            PaError err = Pa_StartStream(mStream);
            if (err != paNoError)
                throw 0;
        }

        void abort()
        {
            if (!Pa_IsStreamStopped(mStream)) {
                PaError err = Pa_AbortStream(mStream);
                if (err != paNoError)
                    throw 0;
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
            const int16_t *source = static_cast<const int16_t *>(mBuffer);

            unsigned long count = std::min<unsigned int>(frameCount, (static_cast<const int16_t *>(mEnd) - source) / mChannels);

            for (size_t i = 0; i < count; ++i) {
                for (int i = 0; i < mChannels; ++i)
                    *target++ = *source++;                
            }

            mBuffer = source;
            return mBuffer == mEnd ? paComplete : paContinue;
        }

        void finishedCallback()
        {
            PortAudioApi *api = static_cast<PortAudioApi *>(mState->mApi);
            mState->set_value();
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
        const void *mBuffer;
        const void *mEnd;

        int mChannels;
        int mSampleRate;
    };

    PortAudioApi::PortAudioApi(Root::Root &root)
        : AudioApiImpl<PortAudioApi>(root)
    {

        root.taskQueue()->addSetupSteps(
            []() {
                PaError err = Pa_Initialize();
                if (err != paNoError) {
                    LOG_ERROR("PortAudio error: " << Pa_GetErrorText(err));
                    return false;
                }
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

    void PortAudioApi::playSoundImpl(PlaybackState &state)
    {
        PortAudioStream &stream = fetchStream(state.mBuffer->mInfo);
        stream.play(state);
    }

    PortAudioApi::PortAudioStream &PortAudioApi::fetchStream(const AudioInfo &info)
    {
        std::unique_lock lock { mMutex };
        auto it = std::ranges::find_if(mStreamPool, [&](const PortAudioStream &stream) { return stream.isCompatible(info); });
        if (it == mStreamPool.end()) {
            return mBusyStreams.emplace_back(info);
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
