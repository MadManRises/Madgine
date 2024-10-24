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

		WritableByteBuffer mapData(const Area &area);

        void reset();

    private:
        std::vector<Area> mFreeList;
        size_t mIndex = 0;
        size_t mSize;
        OpenGLBuffer mBuffer = { GL_UNIFORM_BUFFER };
    };

    struct OpenGLSSBOBuffer {

        OpenGLSSBOBuffer(GLenum target);
        ~OpenGLSSBOBuffer();

		void reset();

        void resize(size_t size);
		WritableByteBuffer mapData();

		size_t offset() const;

    private:
        typename OpenGLSSBOBufferStorage::Area mArea;        
    };

}
}

#endif