#pragma once

namespace Engine {
namespace Resources {

    struct ImageData {
        ImageData(const std::vector<unsigned char> &buffer);
        ~ImageData();

		unsigned char *mBuffer;
        int mWidth, mHeight, mChannels;
    };

}
}