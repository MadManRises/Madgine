#include "../opengllib.h"

#if OPENGL_ES

#    include "../openglrenderwindow.h"
#    include "Modules/uniquecomponent/uniquecomponentcollector.h"
#    include "openglssbobuffer.h"

#    include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    OpenGLSSBOBufferStorage::OpenGLSSBOBufferStorage(int bindingIndex, size_t size)
        : mSize(size)
        , mBuffer(GL_UNIFORM_BUFFER, { nullptr, size * 16 })
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingIndex, mBuffer.handle());
        GL_CHECK();
    }

    OpenGLSSBOBufferStorage::Area OpenGLSSBOBufferStorage::allocate(size_t size)
    {
        assert(size > 0);
        for (auto it = mFreeList.begin(); it != mFreeList.end(); ++it) {
            if (it->mSize == size) {
                Area area = *it;
                mFreeList.erase(it);
                return area;
            }
        }
        Area area = { mIndex,
            size };
        mIndex += size;

        if (mIndex >= mSize)
            std::terminate();
        return area;
    }

    void OpenGLSSBOBufferStorage::deallocate(const Area &area)
    {
        mFreeList.push_back(area);
    }

    WritableByteBuffer OpenGLSSBOBufferStorage::mapData(const Area &area)
    {
        return mBuffer.mapData(area.mIndex * 16, area.mSize * 16);
    }

    void OpenGLSSBOBufferStorage::reset()
    {
        mFreeList.clear();
        mBuffer.reset();
    }

    OpenGLSSBOBuffer::OpenGLSSBOBuffer(GLenum target)
    {
        assert(target = GL_UNIFORM_BUFFER);
    }

    OpenGLSSBOBuffer::~OpenGLSSBOBuffer()
    {
        reset();
    }

    void OpenGLSSBOBuffer::reset()
    {
        if (mArea.mSize > 0) {
            OpenGLSSBOBufferStorage &storage = OpenGLRenderWindow::getSSBOStorage();
            storage.deallocate(mArea);
        }
    }

    void OpenGLSSBOBuffer::resize(size_t size)
    {
        if (size == 0)
            return;
        size = (size - 1) / 16 + 1;
        OpenGLSSBOBufferStorage &storage = OpenGLRenderWindow::getSSBOStorage();
        if (mArea.mSize < size) {
            reset();
            mArea = storage.allocate(size);
        }
    }

    WritableByteBuffer OpenGLSSBOBuffer::mapData()
    {
        OpenGLSSBOBufferStorage &storage = OpenGLRenderWindow::getSSBOStorage();
        GL_LOG("SSBO-Data: " << mArea.mIndex << ", " << mArea.mSize);
        return storage.mapData(mArea);
    }

    size_t OpenGLSSBOBuffer::offset() const
    {
        return mArea.mIndex;
    }

}
}

#endif