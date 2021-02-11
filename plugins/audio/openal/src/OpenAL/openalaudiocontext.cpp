#include "openallib.h"

#include "openalaudiocontext.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"

#include <AL/al.h>
#include <AL/alc.h>

#include "audiobufferloader.h"

UNIQUECOMPONENT(Engine::Audio::OpenALAudioContext)

METATABLE_BEGIN(Engine::Audio::OpenALAudioContext)
METATABLE_END(Engine::Audio::OpenALAudioContext)

namespace Engine {
namespace Audio {

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

    void check_al_errors(const std::string &filename, const std::uint_fast32_t line)
    {
        ALCenum error = alGetError();
        if (error != AL_NO_ERROR) {
            LOG_ERROR("***ERROR*** (" << filename << ": " << line << ")");
            switch (error) {
            case AL_INVALID_NAME:
                LOG_ERROR("AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function");
                break;
            case AL_INVALID_ENUM:
                LOG_ERROR("AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function");
                break;
            case AL_INVALID_VALUE:
                LOG_ERROR("AL_INVALID_VALUE: an invalid value was passed to an OpenAL function");
                break;
            case AL_INVALID_OPERATION:
                LOG_ERROR("AL_INVALID_OPERATION: the requested operation is not valid");
                break;
            case AL_OUT_OF_MEMORY:
                LOG_ERROR("AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory");
                break;
            default:
                LOG_ERROR("UNKNOWN AL ERROR: " << error);
            }
        }
    }

    template <typename alFunction, typename... Params>
    auto alCallImpl(const char *filename, const std::uint_fast32_t line, alFunction function, Params... params)
        -> typename std::enable_if<std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
    {
        function(std::forward<Params>(params)...);
        check_al_errors(filename, line);
    }

    template <typename alFunction, typename... Params>
    auto alCallImpl(const char *filename, const std::uint_fast32_t line, alFunction function, Params... params)
        -> typename std::enable_if<!std::is_same<void, decltype(function(params...))>::value, decltype(function(params...))>::type
    {
        auto ret = function(std::forward<Params>(params)...);
        check_al_errors(filename, line);
        return ret;
    }

    OpenALAudioContext::OpenALAudioContext(App::Application &app)
        : VirtualScope(app)
    {
    }

    OpenALAudioContext::~OpenALAudioContext()
    {
    }

    bool OpenALAudioContext::init()
    {
        mDevice = alcOpenDevice(nullptr);
        assert(mDevice);

        mContext = alcCreateContext(mDevice, nullptr);
        assert(mContext);
        alcMakeContextCurrent(mContext);

        /*AudioBufferLoader::load("Example2");
        auto audioSample = AudioBufferLoader::load("Example");

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
            return false;
        }

        alCall(alGenSources, 1, &source);
        alCall(alSourcef, source, AL_PITCH, 1);
        alCall(alSourcef, source, AL_GAIN, 1.0f);
        alCall(alSource3f, source, AL_POSITION, 0, 0, 0);
        alCall(alSource3f, source, AL_VELOCITY, 0, 0, 0);
        alCall(alSourcei, source, AL_LOOPING, AL_TRUE);

        alCall(alGenBuffers, 1, &buffer);

        alCall(alBufferData, buffer, format, audioSample->mBuffer.mData, audioSample->mBuffer.mSize, audioSample->mInfo.mSampleRate);

        alCall(alSourceQueueBuffers, source, 1, &buffer);

        alCall(alSourcePlay, source);
        */
        return true;
    }

    void OpenALAudioContext::finalize()
    {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(mContext);
        mContext = nullptr;

        alcCloseDevice(mDevice);
        mDevice = nullptr;
    }

}
}
