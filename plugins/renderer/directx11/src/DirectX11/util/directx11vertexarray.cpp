#include "../directx11lib.h"

#include "directx11vertexarray.h"

#include "directx11buffer.h"

#include "directx11program.h"

namespace Engine {
namespace Render {

    static thread_local DirectX11VertexArray *sCurrentBound = nullptr;

    DirectX11VertexArray::DirectX11VertexArray(const std::array<AttributeDescriptor, 7> &attributes)
    {
        uint8_t acc = 1;

        for (const AttributeDescriptor &attribute : attributes) {
            if (attribute) {
                mFormat += acc;

                mAttributes.push_back(attribute);
                if (mStride == 0)
                    mStride = attribute.mStride;
                else {
                    assert(mStride == attribute.mStride);
                }
            }
            acc <<= 1;
        }
    }

    DirectX11VertexArray::DirectX11VertexArray(DirectX11VertexArray &&other)
        : mStride(std::exchange(other.mStride, 0))
        , mFormat(std::exchange(other.mFormat, 0))
        , mAttributes(std::move(other.mAttributes))
        , mVBO(std::exchange(other.mVBO, nullptr))
        , mEBO(std::exchange(other.mEBO, nullptr))
    {
    }

    DirectX11VertexArray::~DirectX11VertexArray()
    {
        reset();
    }

    DirectX11VertexArray &DirectX11VertexArray::operator=(DirectX11VertexArray &&other)
    {
        if (sCurrentBound == &other)
            sCurrentBound = this;
        mAttributes = std::move(other.mAttributes);
        mVBO = std::exchange(other.mVBO, nullptr);
        mEBO = std::exchange(other.mEBO, nullptr);
        mStride = std::exchange(other.mStride, 0);
        mFormat = std::exchange(other.mFormat, 0);
        return *this;
    }

    DirectX11VertexArray::operator bool() const
    {
        return mVBO != nullptr;
    }

    DirectX11VertexArray *DirectX11VertexArray::getCurrent()
    {
        return sCurrentBound;
    }

    void DirectX11VertexArray::reset()
    {
        if (sCurrentBound == this)
            sCurrentBound = nullptr;
        mAttributes.clear();
        mVBO = 0;
        mEBO = 0;
        mFormat = 0;
    }

    void DirectX11VertexArray::bind(DirectX11Program *program)
    {
        assert(sCurrentBound == nullptr);
        sCurrentBound = this;

        DX11_LOG("Bind VAO -> " << mVBO << ", " << mEBO);

        if (mVBO)
            mVBO->bindVertex(mStride);
        if (mEBO)
            mEBO->bindIndex();

        if (program)
            program->bind(this);
    }

    void DirectX11VertexArray::unbind()
    {
        assert(sCurrentBound == this);
        sCurrentBound = nullptr;
    }

    void DirectX11VertexArray::onBindVBO(const DirectX11Buffer *buffer)
    {
        if (sCurrentBound)
            sCurrentBound->mVBO = buffer;
    }

    void DirectX11VertexArray::onBindEBO(const DirectX11Buffer *buffer)
    {
        if (sCurrentBound)
            sCurrentBound->mEBO = buffer;
    }

    std::pair<const DirectX11Buffer *, const DirectX11Buffer *> DirectX11VertexArray::getCurrentBindings()
    {
        if (sCurrentBound) {
            return { sCurrentBound->mVBO, sCurrentBound->mEBO };
        } else {
            return { nullptr, nullptr };
        }
    }

}
}