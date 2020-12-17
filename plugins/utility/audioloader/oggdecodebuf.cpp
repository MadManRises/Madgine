#include "oggdecodebuf.h"

#include "audioinfo.h"

namespace Engine {
namespace Audio {

    template <auto f, typename... Args>
    static auto delegate2(Args... args, void *buf)
    {
        return (static_cast<OggDecodeBuf *>(buf)->*f)(args...);
    }

    template <auto f, typename... Args>
    static auto delegate(void *buf, Args... args)
    {
        return (static_cast<OggDecodeBuf *>(buf)->*f)(args...);
    }

    OggDecodeBuf::OggDecodeBuf(AudioInfo &info, std::unique_ptr<std::basic_streambuf<char>> &&base)
        : mBase(std::move(base))
    {
        setg(mBuffer.data(), mBuffer.data(), mBuffer.data());

        ov_callbacks callbacks;
        callbacks.close_func = nullptr;
        callbacks.read_func = delegate2<&OggDecodeBuf::ogg_read, void *, size_t, size_t>;
        callbacks.seek_func = delegate<&OggDecodeBuf::ogg_seek, int64_t, int32_t>;
        callbacks.tell_func = delegate<&OggDecodeBuf::ogg_tell>;

        auto result = ov_open_callbacks(this, &mFile, nullptr, -1, callbacks);
        if (result)
            std::terminate();

        vorbis_info *vorbisInfo = ov_info(&mFile, -1);

        info.mChannels = vorbisInfo->channels;
        info.mBitsPerSample = 16; //?
        info.mSampleRate = vorbisInfo->rate;
        info.mDuration = std::chrono::microseconds { static_cast<long long>(ov_time_total(&mFile, -1) * 1000000.0f) };
        info.mSampleCount = ov_pcm_total(&mFile, -1);
    }

    OggDecodeBuf::~OggDecodeBuf()
    {
        ov_clear(&mFile);
    }

    OggDecodeBuf::pos_type OggDecodeBuf::seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode)
    {
        LOG("TODO: OggDecodeBuf");
        return pos_type(off_type(-1));
    }

    OggDecodeBuf::pos_type OggDecodeBuf::seekpos(pos_type pos, std::ios_base::openmode mode)
    {
        LOG("TODO: OggDecodeBuf");
        return pos_type(off_type(-1));
    }

    OggDecodeBuf::int_type OggDecodeBuf::overflow(int c)
    {
        LOG("TODO: OggDecodeBuf");
        return traits_type::eof();
    }

    OggDecodeBuf::int_type OggDecodeBuf::underflow()
    {
        int bitstream = 0;
        auto result = ov_read(&mFile, mBuffer.data(), BUFFER_SIZE, 0, 2, 1, &bitstream);
        if (result < 0)
            std::terminate();
        if (result == 0)
            return traits_type::eof();
        setg(mBuffer.data(), mBuffer.data(), mBuffer.data() + result);
        return static_cast<unsigned char>(mBuffer[0]);
    }

    int OggDecodeBuf::sync()
    {
        return 0;
    }

    size_t OggDecodeBuf::ogg_read(void *destination, size_t size1, size_t size2)
    {
        return mBase.readRaw(destination, size1 * size2);
    }

    int32_t OggDecodeBuf::ogg_seek(int64_t to, int32_t type)
    {
        std::ios::seekdir dir;
        switch (type) {
        case SEEK_CUR:
            dir = std::ios_base::cur;
            break;
        case SEEK_END:
            dir = std::ios_base::end;
            break;
        case SEEK_SET:
            dir = std::ios_base::beg;
            break;
        default:
            std::terminate();
        }

        return mBase.seek(to, dir) ? 0 : -1;
    }

    long int OggDecodeBuf::ogg_tell()
    {
        return mBase.tell();
    }

}
}