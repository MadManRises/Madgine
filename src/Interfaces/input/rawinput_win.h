#pragma once

#if WINDOWS

#    define NOMINMAX
#    include "inputevents.h"
#    include <Windows.h>
#    include <hidsdi.h>
#    include <queue>

namespace Engine {
namespace Input {

    struct CapData {
        enum {
            BUTTON,
            FLOAT,
            INT
        } mType;
        bool *mChangedFlag;
        union {
            float *mValue;
            int *mIntValue;
            Input::Key::Key mKey;
        };
    };

    struct RawInputDevice {
        enum Dir {
            UP,
            DOWN
        };
        
        RawInputDevice(HANDLE handle, std::string manufacturer, std::string product, PHIDP_PREPARSED_DATA preparsedData);

        CapData resolveValueCap(USAGE usage);

        CapData resolveButtonCap(USAGE usage);

        bool fetchAxisEvent(AxisEventArgs &arg);
        bool fetchKeyEvent(KeyEventArgs &arg, Dir &dir);

        HANDLE mHandle;
        std::string mManufacturer;
        std::string mProduct;

        PHIDP_PREPARSED_DATA mPreparsedData;

        std::vector<CapData> mCaps;

        struct ControlStick {
            Input::AxisEventArgs::AxisType mType;
            bool mChanged;
            float mAxis1, mAxis2;
        };

        std::vector<ControlStick> mControlSticks;

        std::queue<std::pair<KeyEventArgs, Dir>> mKeyEvents;
        bool mButtonMask[16] = { 0 };

        struct ZAxis {
            bool mChanged;
            float mValue;
        } mZAxis;

        struct DPad {
            bool mChanged;
            int mValue;
        } mDPad;

        struct UnknownValueInput {
            bool dummy1;
            float dummy2;
        } mUnknownValueDummy;
    };

    void setupRawInput(HWND handle);
    Input::RawInputDevice &handleRawInput(HRAWINPUT input);

}
}

#endif