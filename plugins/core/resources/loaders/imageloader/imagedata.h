#pragma once

namespace Engine {
namespace Resources {

    struct MADGINE_IMAGELOADER_EXPORT ImageData {
        ImageData() = default;
        ImageData(const std::vector<unsigned char> &buffer);
        ImageData(ImageData &&other) = default;

		ImageData &operator=(ImageData &&other) = default;

		void clear();        

		std::unique_ptr<unsigned char, void (*)(void *)> mBuffer = { nullptr, nullptr };
        int mWidth, mHeight, mChannels;
    };

}
}