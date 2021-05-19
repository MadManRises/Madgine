#pragma once

#include "Generic/bytebuffer.h"
#include "Interfaces/filesystem/api.h"
#include "Interfaces/filesystem/path.h"

namespace Engine {

struct Heroes2Map {

    Heroes2Map(const Filesystem::Path &p, const Filesystem::Path &p2)
        : mPath(p)
        , mPath2(p2)
    {
        InStream buffer = Filesystem::openFileRead(p, true);
        mBuffer = std::vector<unsigned char> { buffer.iterator(), buffer.end() };
        InStream buffer2 = Filesystem::openFileRead(p2, true);
        mBuffer2 = std::vector<unsigned char> { buffer2.iterator(), buffer2.end() };
    }

    void save()
    {
        OutStream buffer = Filesystem::openFileWrite(mPath, true);
        buffer.writeRaw(mBuffer.data(), mBuffer.size());
    }

    void save2()
    {
        OutStream buffer = Filesystem::openFileWrite(mPath2, true);
        buffer.writeRaw(mBuffer2.data(), mBuffer2.size());
    }

    std::vector<unsigned char> mBuffer, mBuffer2;
    Filesystem::Path mPath, mPath2;
};

}