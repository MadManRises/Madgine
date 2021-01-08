#pragma once

#if WINDOWS

#    define NOMINMAX
#    include "inputevents.h"
#    include <Windows.h>
#    include <hidsdi.h>

namespace Engine {
namespace Input {

    struct CapData {
        bool *mChangedFlag;
        union {
            float *mValue;
            Input::Key::Key mKey;
        };
    };

    struct RawInputDevice {
        RawInputDevice(HANDLE handle, std::string manufacturer, std::string product, PHIDP_PREPARSED_DATA preparsedData);

        CapData resolveValueCap(USAGE usage);

        CapData resolveButtonCap(USAGE usage);

        bool fetchEvent(AxisEventArgs &arg);

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

        struct ZAxis {
            bool mChanged;
            float mValue;
        } mZAxis;

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