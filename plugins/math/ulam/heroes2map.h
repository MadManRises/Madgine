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
        Stream buffer = Filesystem::openFileRead(p, true);
        mBuffer = std::vector<unsigned char> { buffer.iterator(), buffer.end() };
        Stream buffer2 = Filesystem::openFileRead(p2, true);
        mBuffer2 = std::vector<unsigned char> { buffer2.iterator(), buffer2.end() };
    }

    void save()
    {
        Stream buffer = Filesystem::openFileWrite(mPath, true);
        buffer.write(mBuffer.data(), mBuffer.size());
    }

    void save2()
    {
        Stream buffer = Filesystem::openFileWrite(mPath2, true);
        buffer.write(mBuffer2.data(), mBuffer2.size());
    }

    std::vector<unsigned char> mBuffer, mBuffer2;
    Filesystem::Path mPath, mPath2;
};

}