#pragma once

#include "Modules/serialize/streams/buffered_streambuf.h"

#include "Modules/serialize/syncmanager.h"

#include "Modules/serialize/formatter/safebinaryformatter.h"

using namespace Engine::Serialize;

struct Buffer {
    std::array<char, 2048> mBuffer[2];
    size_t mWrittenCount[2] = { 0, 0 };
};

struct TestBuf : Engine::Serialize::buffered_streambuf {
    TestBuf(Buffer &buffer, Engine::Serialize::SyncManager &mgr, Engine::Serialize::ParticipantId id)
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

struct TestManager : Engine::Serialize::SyncManager {
    TestManager(const char *id)
        : SyncManager("Test-Manager_"s + id)
    {
    }

    BufferedInOutStream &setBuffer(Buffer &buffer, bool slave, bool shareState = true)
    {
        if (slave) {
            setSlaveStream(Engine::Serialize::BufferedInOutStream { std::make_unique<TestBuf>(buffer, *this, 0) }, shareState, std::chrono::milliseconds { 1000 });
            return *getSlaveStream();
        } else {
            addMasterStream(Engine::Serialize::BufferedInOutStream { std::make_unique<TestBuf>(buffer, *this, 1) }, shareState);
            return const_cast<BufferedInOutStream &>(*getMasterStreams().find(1));
        }
    }
};