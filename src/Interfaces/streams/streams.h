#pragma once

namespace Engine {

typedef std::istream::pos_type pos_type;

struct INTERFACES_EXPORT InStream {
    InStream();
    InStream(std::unique_ptr<std::streambuf> &&buffer);
    InStream(InStream &&other);
    ~InStream();

    template <typename T>
    InStream &operator>>(T &t)
    {
        mStream >> t;
        return *this;
    }

    std::istreambuf_iterator<char> iterator();
    std::istreambuf_iterator<char> end();

    bool readRaw(void *buffer, size_t size);

    operator bool() const;

    pos_type tell();
    void seek(pos_type p);

    void skipWs();

	std::unique_ptr<std::streambuf> release();

protected:
    InStream(std::streambuf *buffer);

    std::streambuf &buffer() const;

    std::istream mStream;

    friend struct OutStream;

private:
    bool mOwning = true;
};

struct INTERFACES_EXPORT OutStream {
    OutStream();
    OutStream(std::unique_ptr<std::streambuf> &&buffer);
    OutStream(OutStream &&other);
    ~OutStream();

    template <typename T>
    OutStream &operator<<(const T &t)
    {
        mStream << t;
        return *this;
    }

    void writeRaw(const void *data, size_t count);

    operator bool() const;

    std::unique_ptr<std::streambuf> release();

    void pipe(InStream &in);

protected:
    std::streambuf &buffer() const;

    std::ostream mStream;
};

struct INTERFACES_EXPORT Stream : InStream, OutStream {
    Stream(std::unique_ptr<std::streambuf> &&buffer);

    using InStream::buffer;
};

}