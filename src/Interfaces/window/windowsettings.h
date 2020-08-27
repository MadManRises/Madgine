#pragma once

namespace Engine {
namespace Window {

    struct WindowSettings {
        uintptr_t mHandle = 0;

        InterfacesVector mSize = { 800, 600 };

        std::optional<InterfacesVector> mPosition;

        const char *mTitle = "Interfaces - Window";

        bool mHidden = false;
        bool mHeadless = false;
    };

}
}