#pragma once

namespace Engine {
namespace Resources {

    struct ImageData {
        ImageData() = default;
        ImageData(const std::vector<unsigned char> &buffer);
        ImageData(ImageData &&other);
        ~ImageData();

		ImageData &operator=(ImageData &&other);

		void clear();

		unsigned char *mBuffer = nullptr;
        int mWidth, mHeight, mChannels;
    };

}
}