#pragma once

#if OPENGL_ES

#    include "openglbuffer.h"

namespace Engine {
namespace Render {

    struct OpenGLSSBOBufferStorage {

		OpenGLSSBOBufferStorage() = default;
        OpenGLSSBOBufferStorage(int bindingIndex, size_t size);

		OpenGLSSBOBufferStorage(OpenGLSSBOBufferStorage &&) = default;

		OpenGLSSBOBufferStorage &operator = (OpenGLSSBOBufferStorage &&) = default;

        struct Area {
            size_t mIndex;
            size_t mSize;
        };

        Area allocate(size_t size);
        void deallocate(const Area &area);

		void setData(const void *data, const Area &area);

    private:
        std::vector<Area> mFreeList;
        size_t mIndex = 0;
        size_t mSize;
        OpenGLBuffer mBuffer;
    };

    struct OpenGLSSBOBuffer {

        OpenGLSSBOBuffer() = default;
        ~OpenGLSSBOBuffer();

		void reset();

		void setData(const ByteBuffer &data);

		size_t offset() const;

    private:
        typename OpenGLSSBOBufferStorage::Area mArea;        
    };

}
}

#endif