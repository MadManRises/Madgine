#include "../opengllib.h"

#include "openglheapallocator.h"

#include "Madgine/render/ptr.h"

namespace Engine {
namespace Render {

    OpenGLHeapAllocator::OpenGLHeapAllocator()
    {
    }

    Block OpenGLHeapAllocator::allocate(size_t size, size_t alignment)
    {
        GPUPtr<void> ptr;

        glGenBuffers(1, &ptr.mBuffer.mIndex);
        GL_CHECK();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ptr.mBuffer);
        GL_CHECK();

        glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_COPY);
        GL_CHECK();

        return { reinterpret_cast<void *&>(ptr), size };
    }

    void OpenGLHeapAllocator::deallocate(Block block)
    {
        GPUPtr<void> ptr = reinterpret_cast<GPUPtr<void> &>(block.mAddress);
        assert(ptr.mOffset == 0 && ptr.mBuffer != 0);

        glDeleteBuffers(1, &ptr.mBuffer.mIndex);
        GL_CHECK();
    }

    OpenGLMappedHeapAllocator::OpenGLMappedHeapAllocator()
    {
    }

    Block OpenGLMappedHeapAllocator::allocate(size_t size, size_t alignment)
    {
        GLuint handle;

        glCreateBuffers(1, &handle);
        GL_CHECK();

        glNamedBufferStorage(handle, size, nullptr, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT);
        GL_CHECK();
        
        void *ptr = glMapNamedBufferRange(handle, 0, size, GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT | GL_MAP_WRITE_BIT);
        GL_CHECK();

        mPages.push_back({ handle,
            ptr,
            size});

        return { ptr, size };
    }

    void OpenGLMappedHeapAllocator::deallocate(Block block)
    {
        auto it = std::ranges::find(mPages, block.mAddress, &Page::mMappedAddress);
        assert(it != mPages.end());

        glDeleteBuffers(1, &it->mBuffer);

        mPages.erase(it);
    }

    std::pair<GLuint, size_t> OpenGLMappedHeapAllocator::resolve(void *ptr)
    {
        auto it = std::ranges::find_if(mPages, [=](const Page &page) {
            return page.mMappedAddress <= ptr && ptr < static_cast<std::byte *>(page.mMappedAddress) + page.mSize;
        });
        assert(it != mPages.end());
        return { it->mBuffer, static_cast<std::byte *>(ptr) - static_cast<std::byte *>(it->mMappedAddress) };
    }

}
}