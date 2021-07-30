#include "../opengllib.h"

#if OPENGL_ES

#    include "openglssbobuffer.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"
#include "../openglrendercontext.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    OpenGLSSBOBufferStorage::OpenGLSSBOBufferStorage(int bindingIndex, size_t size)
        : mSize(size)
    {
        mBuffer = { GL_UNIFORM_BUFFER, size * 16 };
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

    OpenGLSSBOBuffer::~OpenGLSSBOBuffer()
    {
        reset();
    }

    void OpenGLSSBOBuffer::reset()
    {
        if (mArea.mSize > 0) {
            OpenGLSSBOBufferStorage &storage = OpenGLRenderContext::getSingleton().mSSBOBuffer;
            storage.deallocate(mArea);
		}
    }

    void OpenGLSSBOBuffer::resize(size_t size) {
        if (size == 0)
            return;
        size = (size - 1) / 16 + 1;
        OpenGLSSBOBufferStorage &storage = OpenGLRenderContext::getSingleton().mSSBOBuffer;
        if (mArea.mSize < size) {
            reset();
            mArea = storage.allocate(size);
        }
    }

    WritableByteBuffer OpenGLSSBOBuffer::mapData()
    {
        OpenGLSSBOBufferStorage &storage = OpenGLRenderContext::getSingleton().mSSBOBuffer;
        GL_LOG("SSBO-Data: " << size);
        return storage.mapData(mArea);		
    }

    size_t OpenGLSSBOBuffer::offset() const
    {
        return mArea.mIndex;
    }

}
}

#endif