#pragma once

namespace Engine {
namespace Window {
	
    struct WindowSettings {
        uintptr_t mHandle = 0;

        struct WindowVector {
            int x;
            int y;
        };

        WindowVector mSize = { 800, 600 };

        std::optional<WindowVector> mPosition;

        const char *mTitle = "Interfaces - Window";

        bool mHidden = false;
        bool mHeadless = false;
    };

}
}