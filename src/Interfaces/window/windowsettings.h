#pragma once

namespace Engine {
namespace Window {

    struct WindowData {
        InterfacesVector mSize = { 800, 600 };
        InterfacesVector mPosition = { -1, -1 };
        bool mFullScreen = false;
    };

    struct WindowSettings {
        uintptr_t mHandle = 0;

        WindowData mData;

        const char *mTitle = "Interfaces - Window";

        bool mHidden = false;
        bool mHeadless = false;
    };

}
}