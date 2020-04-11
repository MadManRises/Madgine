#pragma once

#include "Modules/serialize/streams/buffered_streambuf.h"

#include "Modules/serialize/syncmanager.h"

#include "Modules/serialize/formatter/safebinaryformatter.h"

using namespace Engine::Serialize;

struct Buffer {
    std::array<char, 4096> mBuffer[2];
    size_t mWrittenCount[2] = { 0, 0 };
};

struct BufferedTestBuf : Engine::Serialize::buffered_streambuf {
    BufferedTestBuf(Buffer &buffer, Engine::Serialize::SyncManager &mgr, Engine::Serialize::ParticipantId id)
        : buffered_streambuf(std::make_unique<SafeBinaryFormatter>(), mgr, id)
        , mBuffer(buffer)
    {
    }

    virtual Engine::StreamError getError()
    {
        return Engine::WOULD_BLOCK;
    }

    virtual int recv(char *buffer, size_t count)
    {
        size_t index = isMaster(StreamMode::READ) ? 0 : 1;

        size_t avail = mBuffer.mWrittenCount[index] - mReadOffset;
        size_t readCount = std::min(count, avail);
        if (readCount == 0)
            return -1;
        std::memcpy(buffer, mBuffer.mBuffer[index].data() + mReadOffset, readCount);
        mReadOffset += readCount;
        return static_cast<int>(readCount);
    }

    virtual int send(char *buffer, size_t count)
    {
        size_t index = isMaster(StreamMode::WRITE) ? 1 : 0;

        assert(mBuffer.mWrittenCount[index] + count <= mBuffer.mBuffer[index].size());
        std::memcpy(mBuffer.mBuffer[index].data() + mBuffer.mWrittenCount[index], buffer, count);
        mBuffer.mWrittenCount[index] += count;
        return static_cast<int>(count);
    }

    Buffer &mBuffer;
    size_t mReadOffset = 0;
};

struct TestBuf : Engine::Serialize::SerializeStreambuf {
    TestBuf(Buffer &buffer, Engine::Serialize::SyncManager &mgr)
        : SerializeStreambuf(std::make_unique<SafeBinaryFormatter>())
        , mBuffer(buffer)
    {
    }

    TestBuf(Buffer &buffer, Engine::Serialize::SyncManager &mgr, Engine::Serialize::ParticipantId id)
        : SerializeStreambuf(std::make_unique<SafeBinaryFormatter>(), mgr, id)
        , mBuffer(buffer)
    {
        size_t index = isMaster(StreamMode::WRITE) ? 1 : 0;
        setp(mBuffer.mBuffer[index].data(), mBuffer.mBuffer[index].data());

        setg(mBuffer.mBuffer[1 - index].data(), mBuffer.mBuffer[1 - index].data(), mBuffer.mBuffer[1 - index].data() + mBuffer.mWrittenCount[1 - index]);
    }

    TestBuf(Buffer &buffer)
        : SerializeStreambuf(std::make_unique<SafeBinaryFormatter>())
        , mBuffer(buffer)
    {
        size_t index = 1;
        setp(mBuffer.mBuffer[index].data(), mBuffer.mBuffer[index].data());

        setg(mBuffer.mBuffer[index].data(), mBuffer.mBuffer[index].data(), mBuffer.mBuffer[index].data() + mBuffer.mWrittenCount[index]);
    }

    SerializeStreambuf::int_type overflow(int c) override
    {
        if (c != EOF) {
            size_t index = 1;

            setp(pptr(), pptr() + 1);
            ++mBuffer.mWrittenCount[index];

            *pptr() = c;
            pbump(1);
            return c;
        }
        return traits_type::eof();
    }

    SerializeStreambuf::int_type underflow() override
    {
        size_t index = 1;
        if (mBuffer.mWrittenCount[index] > (egptr() - eback())) {
            setg(eback(), gptr(), eback() + mBuffer.mWrittenCount[index]);
            SerializeStreambuf::int_type c = *gptr();
            //gbump(1);
            return c;
        }
        return traits_type::eof();
    }

    Buffer &mBuffer;
    size_t mReadOffset = 0;
};

struct TestManager : Engine::Serialize::SyncManager {
    TestManager(const char *id)
        : SyncManager("Test-Manager_"s + id)
    {
    }

    BufferedInOutStream &setBuffer(Buffer &buffer, bool slave, bool shareState = true)
    {
        if (slave) {
            setSlaveStream(Engine::Serialize::BufferedInOutStream { std::make_unique<BufferedTestBuf>(buffer, *this, 0) }, shareState, std::chrono::milliseconds { 1000 });
            return *getSlaveStream();
        } else {
            addMasterStream(Engine::Serialize::BufferedInOutStream { std::make_unique<BufferedTestBuf>(buffer, *this, 1) }, shareState);
            return const_cast<BufferedInOutStream &>(*getMasterStreams().find(1));
        }
    }
};