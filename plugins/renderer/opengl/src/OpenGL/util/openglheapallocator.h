#pragma once

#include "Generic/allocator/concepts.h"

namespace Engine {
namespace Render {

    struct OpenGLHeapAllocator {

        static constexpr size_t goodSize = 256 * 1024 * 1024; // 256MB

        OpenGLHeapAllocator(
#if OPENGL_ES
            GLenum target
#endif
        );

        Memory::Block allocate(size_t size, size_t alignment = 1);
        void deallocate(Memory::Block block);

        std::pair<GLuint, uintptr_t> resolve(void *ptr);

    private:
#if OPENGL_ES
        GLenum mTarget;
#endif
    };

    struct OpenGLMappedHeapAllocator {

#if OPENGL_ES
        static constexpr size_t goodSize = 8 * 1024 * 1024; // 8MB
#else
        static constexpr size_t goodSize = 32 * 1024 * 1024; // 32MB
#endif

        OpenGLMappedHeapAllocator(
#if OPENGL_ES
            GLenum target
#endif
        );

        Memory::Block allocate(size_t size, size_t alignment = 1);
        void deallocate(Memory::Block block);

        std::pair<GLuint, uintptr_t> resolve(void *ptr);

    private:
#if !OPENGL_ES
        struct Page {
            GLuint mBuffer;
            void *mMappedAddress;
            size_t mSize;
        };
        std::vector<Page> mPages;
#else
        GLenum mTarget;
#endif
    };

}
}