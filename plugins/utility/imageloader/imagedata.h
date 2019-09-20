#pragma once

namespace Engine {
namespace Resources {

    struct ImageData {
        ImageData(const char *path);
        ~ImageData();

		unsigned char *mBuffer;
        int mWidth, mHeight, mChannels;
    };

}
}