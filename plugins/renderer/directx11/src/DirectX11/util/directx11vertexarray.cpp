#include "../directx11lib.h"

#include "directx11vertexarray.h"

#include "directx11buffer.h"

#include "directx11program.h"

namespace Engine {
namespace Render {

    DirectX11VertexArray::DirectX11VertexArray(const DirectX11Buffer &vertex, const DirectX11Buffer &index, std::array<AttributeDescriptor, 7> (*attributes)())
        : mAttributes(attributes)
        , mVBO(&vertex)
        , mEBO(&index)
    {
        uint8_t acc = 1;

        for (const AttributeDescriptor &attribute : attributes()) {
            if (attribute) {
                mFormat += acc;

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

    void DirectX11VertexArray::reset()
    {
        mAttributes = nullptr;
        mVBO = 0;
        mEBO = 0;
        mFormat = 0;
    }

    void DirectX11VertexArray::bind() const 
    {
        DX11_LOG("Bind VAO -> " << mVBO << ", " << mEBO);

        if (mVBO)
            mVBO->bindVertex(mStride);
        if (mEBO)
            mEBO->bindIndex();
    }

}
}