#include "../opengllib.h"

#include "openglbuffer.h"

#include "Generic/bytebuffer.h"

#if OPENGL_ES
#    include "openglvertexarray.h"
#endif

namespace Engine {
namespace Render {

    OpenGLBuffer::OpenGLBuffer(GLenum target, const ByteBuffer &data)
        : mTarget(target)
    {
        glGenBuffers(1, &mHandle);
        GL_CHECK();

        setData(data);
    }

    OpenGLBuffer::OpenGLBuffer(OpenGLBuffer &&other)
        : mHandle(std::exchange(other.mHandle, 0))
        , mTarget(std::exchange(other.mTarget, 0))
        , mSize(std::exchange(other.mSize, 0))
    {
    }

    OpenGLBuffer::~OpenGLBuffer()
    {
        reset();
    }

    OpenGLBuffer &OpenGLBuffer::operator=(OpenGLBuffer &&other)
    {
        std::swap(mHandle, other.mHandle);
        std::swap(mTarget, other.mTarget);
        std::swap(mSize, other.mSize);
        return *this;
    }

    OpenGLBuffer::operator bool() const
    {
        return mSize > 0;
    }

    void OpenGLBuffer::bind() const
    {
        glBindBuffer(mTarget, mHandle);
        GL_LOG("Bind Buffer " << mTarget << " -> " << mHandle);
        GL_CHECK();
#if OPENGL_ES
        if (mTarget == GL_ELEMENT_ARRAY_BUFFER)
            OpenGLVertexArray::onBindEBO(mHandle);
        if (mTarget == GL_ARRAY_BUFFER)
            OpenGLVertexArray::onBindVBO(mHandle);
#endif
    }

    void OpenGLBuffer::reset()
    {
        if (mHandle) {
            glDeleteBuffers(1, &mHandle);
            GL_CHECK();
            mHandle = 0;
            mTarget = 0;
        }
    }

    void OpenGLBuffer::setData(const ByteBuffer &data)
    {
        bind();
        glBufferData(mTarget, data.mSize, data.mData, data.mData ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
        GL_CHECK();
        mSize = data.mSize;
    }

    void OpenGLBuffer::resize(size_t size)
    {
        if (size != mSize) {
            setData({ nullptr, size });
        }
    }

    WritableByteBuffer OpenGLBuffer::mapData(size_t offset, size_t size)
    {

        if (size == 0)
            size = mSize;

#if !WEBGL
        struct UnmapDeleter {
            OpenGLBuffer *mSelf;

            void operator()(void *p)
            {
                mSelf->bind();
                auto result = glUnmapBuffer(mSelf->mTarget);
                assert(result);
                GL_CHECK();
            }
        };
        bind();
        void *data = glMapBufferRange(mTarget, offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
        GL_CHECK();
        assert(data);

        std::unique_ptr<void, UnmapDeleter> dataBuffer { data, { this } };

        return { std::move(dataBuffer), size };
#else
        struct MapHelper {
            std::unique_ptr<std::byte[]> mPtr;
            OpenGLBuffer *mSelf;
            size_t mOffset;
            size_t mSize;

            MapHelper(OpenGLBuffer *self, size_t offset, size_t size)
                : mPtr(std::make_unique<std::byte[]>(size))
                , mSelf(self)
                , mOffset(offset)
                , mSize(size)
            {
            }

            MapHelper(MapHelper &&other)
                : mPtr(std::move(other.mPtr))
                , mSelf(other.mSelf)
                , mOffset(other.mOffset)
                , mSize(other.mSize)
            {
            }

            ~MapHelper()
            {
                if (mPtr)
                    mSelf->setSubData(mOffset, { mPtr.get(), mSize });
            }
        };

        MapHelper dataBuffer { this, offset, size };

        return { std::move(dataBuffer), size, dataBuffer.mPtr.get() };
#endif
    }

    void OpenGLBuffer::setSubData(unsigned int offset, const ByteBuffer &data)
    {
        bind();
        glBufferSubData(mTarget, offset, data.mSize, data.mData);
        GL_LOG("Buffer-SubData: " << offset << ", " << data.mSize);
        GL_CHECK();
    }

    GLuint OpenGLBuffer::handle()
    {
        return mHandle;
    }

}
}