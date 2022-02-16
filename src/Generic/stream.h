#pragma once

namespace Engine {

typedef std::istream::pos_type pos_type;
typedef std::istream::off_type off_type;

struct Stream {
    Stream()
        : mStream(nullptr)
    {
    }
    Stream(std::unique_ptr<std::streambuf> buffer)
        : mStream(buffer.release())
    {
    }
    Stream(Stream &&other)
        : mStream(other.mStream.rdbuf(nullptr))
    {
    }

    ~Stream()
    {
        delete mStream.rdbuf(nullptr);
    }

    Stream& operator=(Stream&& other) {
        mStream.rdbuf(other.mStream.rdbuf(mStream.rdbuf()));
        return *this;
    }

    template <typename T>
    Stream &operator>>(T &t)
    {
        if constexpr (std::is_enum_v<T>) {
            /* std::underlying_type_t<T> */ int64_t val;
            mStream >> val;
            t = static_cast<T>(val);
        } else {
            mStream >> t;
        }
        return *this;
    }

    std::istreambuf_iterator<char> iterator()
    {
        return std::istreambuf_iterator<char>(mStream);
    }
    std::istreambuf_iterator<char> end()
    {
        return std::istreambuf_iterator<char>();
    }

    size_t read(void *buffer, size_t size)
    {
        mStream.read(static_cast<char *>(buffer), size);
        return mStream.gcount();
    }

    pos_type tell()
    {
        return mStream.tellg();
    }
    std::streamsize gcount()
    {
        return mStream.gcount();
    }
    bool seek(pos_type p)
    {
        return static_cast<bool>(mStream.seekg(p));
    }
    bool seek(off_type p, std::ios::seekdir dir)
    {
        return static_cast<bool>(mStream.seekg(p, dir));
    }

    std::ios_base::iostate state() const
    {
        return mStream.rdstate();
    }

    void clear()
    {
        mStream.clear();
    }

    void setState(std::ios_base::iostate state)
    {
        mStream.setstate(state);
    }

    void skipWs(bool overwrite = false)
    {
        if (overwrite || (mStream.flags() & std::ios_base::skipws)) {
            mStream >> std::ws;
        }
    }

    template <typename T>
    Stream &operator<<(const T &t)
    {
        if constexpr (std::is_enum_v<T>) {
            mStream << static_cast<std::underlying_type_t<T>>(t);
        } else {
            mStream << t;
        }
        return *this;
    }

    void write(const void *data, size_t count)
    {
        mStream.write(static_cast<const char *>(data), count);
    }

    template <typename T>
    void write(const T &t)
    {
        write(&t, sizeof(T));
    }

    explicit operator bool() const
    {
        return mStream.rdbuf() != nullptr && static_cast<bool>(mStream);
    }

    std::unique_ptr<std::streambuf> release()
    {
        return std::unique_ptr<std::streambuf>(mStream.rdbuf(nullptr));
    }

    void pipe(Stream &in)
    {
        mStream << &in.buffer();
    }

    std::iostream& stream() {
        return mStream;
    }

    std::streambuf &buffer() const
    {
        return *mStream.rdbuf();
    }

protected:
    std::iostream mStream;
};

/*struct Stream : InStream, OutStream {
    Stream(std::unique_ptr<std::streambuf> &&buffer)
        : InStream(buffer.get())
        , OutStream(std::move(buffer))
    {
    }

    using InStream::buffer;
};*/
}