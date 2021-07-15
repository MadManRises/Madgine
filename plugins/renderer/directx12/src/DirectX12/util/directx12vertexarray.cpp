#include "../directx12lib.h"

#include "directx12vertexarray.h"

#include "directx12buffer.h"

#include "directx12program.h"

namespace Engine {
namespace Render {

    static thread_local DirectX12VertexArray *sCurrentBound = nullptr;

    DirectX12VertexArray::DirectX12VertexArray(const std::array<AttributeDescriptor, 7> &attributes)
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

    DirectX12VertexArray::DirectX12VertexArray(DirectX12VertexArray &&other)
        : mStride(std::exchange(other.mStride, 0))
        , mFormat(std::exchange(other.mFormat, 0))
        , mAttributes(std::move(other.mAttributes))
        , mVBO(std::exchange(other.mVBO, nullptr))
        , mEBO(std::exchange(other.mEBO, nullptr))
    {
    }

    DirectX12VertexArray::~DirectX12VertexArray()
    {
        reset();
    }

    DirectX12VertexArray &DirectX12VertexArray::operator=(DirectX12VertexArray &&other)
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

    DirectX12VertexArray::operator bool() const
    {
        return mVBO != nullptr;
    }

    DirectX12VertexArray *DirectX12VertexArray::getCurrent()
    {
        return sCurrentBound;
    }

    void DirectX12VertexArray::reset()
    {
        if (sCurrentBound == this)
            sCurrentBound = nullptr;
        mAttributes.clear();
        mVBO = 0;
        mEBO = 0;
        mFormat = 0;
    }

    void DirectX12VertexArray::bind(DirectX12Program *program)
    {
        assert(sCurrentBound == nullptr);
        sCurrentBound = this;

        DX12_LOG("Bind VAO -> " << mVBO << ", " << mEBO);

        if (mVBO)
            mVBO->bindVertex(mStride);
        if (mEBO)
            mEBO->bindIndex();

        if (program)
            program->bind(this);
    }

    void DirectX12VertexArray::unbind()
    {
        assert(sCurrentBound == this);
        sCurrentBound = nullptr;
    }

    void DirectX12VertexArray::onBindVBO(const DirectX12Buffer *buffer)
    {
        if (sCurrentBound)
            sCurrentBound->mVBO = buffer;
    }

    void DirectX12VertexArray::onBindEBO(const DirectX12Buffer *buffer)
    {
        if (sCurrentBound)
            sCurrentBound->mEBO = buffer;
    }

    std::pair<const DirectX12Buffer *, const DirectX12Buffer *> DirectX12VertexArray::getCurrentBindings()
    {
        if (sCurrentBound) {
            return { sCurrentBound->mVBO, sCurrentBound->mEBO };
        } else {
            return { nullptr, nullptr };
        }
    }

}
}