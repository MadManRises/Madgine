#pragma once

#include "texturedescriptor.h"

#include "Meta/math/vector2i.h"

namespace Engine {
namespace Render {

    struct Texture {

        Texture() = default;

        Texture(TextureType type, DataFormat format, const Vector2i &size = { 0, 0 })
            : mType(type)
            , mFormat(format)
            , mSize(size)
        {
        }

        Texture(Texture &&other)
            : mType(other.mType)
            , mFormat(other.mFormat)
            , mSize(other.mSize)
        {
            mTextureHandle = std::exchange(other.mTextureHandle, 0);
        }

        Texture &operator=(Texture &&other)
        {
            std::swap(mTextureHandle, other.mTextureHandle);
            std::swap(mType, other.mType);
            std::swap(mFormat, other.mFormat);
            std::swap(mSize, other.mSize);
            return *this;
        }

        TextureHandle handle() const
        {
            return mTextureHandle;
        }

        const Vector2i &size() const
        {
            return mSize;
        }

        TextureDescriptor descriptor() const
        {
            return { mTextureHandle, mType };
        }

        DataFormat format() const
        {
            return mFormat;
        }

        TextureType type() const
        {
            return mType;
        }

    protected:
        TextureHandle mTextureHandle = 0;
        TextureType mType;
        Vector2i mSize = { 0, 0 };
        DataFormat mFormat;
    };

}
}