#include "openallib.h"

#include "openalaudiocontext.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "audiobufferloader.h"

UNIQUECOMPONENT(Engine::Audio::OpenALAudioContext)

METATABLE_BEGIN(Engine::Audio::OpenALAudioContext)
METATABLE_END(Engine::Audio::OpenALAudioContext)

namespace Engine {
namespace Audio {

    OpenALAudioContext::OpenALAudioContext(Base::Application &app)
        : VirtualScope(app)
    {
    }

    OpenALAudioContext::~OpenALAudioContext()
    {
    }

    Threading::Task<bool> OpenALAudioContext::init()
    {
        mDevice = std::unique_ptr<ALCdevice, Functor<&alcCloseDevice>> { alcOpenDevice(nullptr) };
        assert(mDevice);

        mContext = std::unique_ptr<ALCcontext, Functor<&alcDestroyContext>> { alcCreateContext(mDevice.get(), nullptr) };
        assert(mContext);
        alcMakeContextCurrent(mContext.get());

        AudioBufferLoader::Handle audioSample;

        bool result = co_await audioSample.load("Example2");
        assert(result);

        //AudioBufferLoader::load("Example2");
        //auto audioSample = AudioBufferLoader::load("Example");

        ALuint source;
        ALuint buffer;
        ALenum format;

        if (audioSample->mInfo.mChannels == 1 && audioSample->mInfo.mBitsPerSample == 8)
            format = AL_FORMAT_MONO8;
        else if (audioSample->mInfo.mChannels == 1 && audioSample->mInfo.mBitsPerSample == 16)
            format = AL_FORMAT_MONO16;
        else if (audioSample->mInfo.mChannels == 2 && audioSample->mInfo.mBitsPerSample == 8)
            format = AL_FORMAT_STEREO8;
        else if (audioSample->mInfo.mChannels == 2 && audioSample->mInfo.mBitsPerSample == 16)
            format = AL_FORMAT_STEREO16;
        else {
            LOG_ERROR("unrecognised ogg format: " << audioSample->mInfo.mChannels << " channels, " << audioSample->mInfo.mBitsPerSample << " bps");
            co_return false;
        }

        alGenSources(1, &source);
        AL_CHECK();
        alSourcef(source, AL_PITCH, 1);
        AL_CHECK();
        alSourcef(source, AL_GAIN, 1.0f);
        AL_CHECK();
        alSource3f(source, AL_POSITION, 0, 0, 0);
        AL_CHECK();
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        AL_CHECK();
        alSourcei(source, AL_LOOPING, AL_TRUE);
        AL_CHECK();

        alGenBuffers(1, &buffer);
        AL_CHECK();

        alBufferData(buffer, format, audioSample->mBuffer.mData, audioSample->mBuffer.mSize, audioSample->mInfo.mSampleRate);
        AL_CHECK();

        alSourceQueueBuffers(source, 1, &buffer);
        AL_CHECK();

        alSourcePlay(source);
        AL_CHECK();

        co_return true;
    }

    Threading::Task<void> OpenALAudioContext::finalize()
    {
        alcMakeContextCurrent(nullptr);

        mContext.reset();
        mDevice.reset();

        co_return;
    }

    std::string_view OpenALAudioContext::key() const
    {
        return "OpenALAudioContext";
    }

}
}
