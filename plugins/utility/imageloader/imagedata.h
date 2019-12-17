#pragma once

namespace Engine {
namespace Resources {

    struct ImageData {
        ImageData() = default;
        ImageData(const std::vector<unsigned char> &buffer);
        ImageData(ImageData &&other);
        ~ImageData();

		void clear();

		unsigned char *mBuffer = nullptr;
        int mWidth, mHeight, mChannels;
    };

}
}