#pragma once

namespace Engine {
namespace Input {
    namespace Key {
        enum Key : uint8_t {
            // Control Keys
            Backspace = 0x08,
            Tabulator = 0x09,
            Clear = 0x0c,
            Return = 0x0d,
            Shift = 0x10,
            Control = 0x11,
            Alt = 0x12,
            Pause = 0x13,
            Capslock = 0x14,
            Escape = 0x1b,
            PageUp = 0x21,
            PageDown = 0x22,
            End = 0x23,
            Home = 0x24,
            LeftArrow = 0x25,
            UpArrow = 0x26,
            RightArrow = 0x27,
            DownArrow = 0x28,
            Select = 0x29,
            Print = 0x2a,
            Execute = 0x2b,
            PrintScreen = 0x2c,
            Insert = 0x2d,
            Delete = 0x2e,
            Help = 0x2f,
            LWin = 0x5b,
            RWin = 0x5c,
            NumLock = 0x90,
            ScrollLock = 0x91,
            LShift = 0xa0,
            RShift = 0xa1,
            LControl = 0xa2,
            RControl = 0xa3,
            LAlt = 0xa4,
            RAlt = 0xa5,

            // Alpha
            Space = 0x20,
            A = 0x41,
            B = 0x42,
            C = 0x43,
            D = 0x44,
            E = 0x45,
            F = 0x46,
            G = 0x47,
            H = 0x48,
            I = 0x49,
            J = 0x4a,
            K = 0x4b,
            L = 0x4c,
            M = 0x4d,
            N = 0x4e,
            O = 0x4f,
            P = 0x50,
            Q = 0x51,
            R = 0x52,
            S = 0x53,
            T = 0x54,
            U = 0x55,
            V = 0x56,
            W = 0x57,
            X = 0x58,
            Y = 0x59,
            Z = 0x5a,

            OEM1 = 0xba, // US-Layout: ;:
            Plus = 0xbb,
            Comma = 0xbc,
            Minus = 0xbd,
            Period = 0xbe,
            OEM2 = 0xbf, // US-Layout: /?
            OEM3 = 0xc0, // US-Layout: `~
            OEM4 = 0xdb, // US-Layout: [{
            OEM5 = 0xdc, // US-Layout: \|
            OEM6 = 0xdd, // US-Layout: ]}
            OEM7 = 0xde, // US-Layout: '"
            OEM8 = 0xdf,

            // Num
            Alpha0 = 0x30,
            Alpha1 = 0x31,
            Alpha2 = 0x32,
            Alpha3 = 0x33,
            Alpha4 = 0x34,
            Alpha5 = 0x35,
            Alpha6 = 0x36,
            Alpha7 = 0x37,
            Alpha8 = 0x38,
            Alpha9 = 0x39,

            Num0 = 0x60,
            Num1 = 0x61,
            Num2 = 0x62,
            Num3 = 0x63,
            Num4 = 0x64,
            Num5 = 0x65,
            Num6 = 0x66,
            Num7 = 0x67,
            Num8 = 0x68,
            Num9 = 0x69,

            // Special
            NumMulitply = 0x6a,
            NumAdd = 0x6b,
            NumSeparator = 0x6c,
            NumSubtract = 0x6d,
            NumDecimal = 0x6e,
            NumDivide = 0x6f,

            F1 = 0x70,
            F2 = 0x71,
            F3 = 0x72,
            F4 = 0x73,
            F5 = 0x74,
            F6 = 0x75,
            F7 = 0x76,
            F8 = 0x77,
            F9 = 0x78,
            F10 = 0x79,
            F11 = 0x7a,
            F12 = 0x7b,
            F13 = 0x7c,
            F14 = 0x7d,
            F15 = 0x7e,
            F16 = 0x7f,
            F17 = 0x80,
            F18 = 0x81,
            F19 = 0x82,
            F20 = 0x83,
            F21 = 0x84,
            F22 = 0x85,
            F23 = 0x86,
            F24 = 0x87
        };
    }

    namespace MouseButton {
        enum MouseButton : unsigned char {
            NO_BUTTON,
            LEFT_BUTTON,
            RIGHT_BUTTON,
            MIDDLE_BUTTON,
            BUTTON_COUNT
        };
    }

    struct KeyEventArgs {
        Key::Key scancode;
        char text;

        KeyEventArgs(Key::Key key, char text = 0)
            : scancode(key)
            , text(text)
        {
        }
    };

    struct PointerEventArgs {
        PointerEventArgs(const InterfacesVector &pos, MouseButton::MouseButton button)
            : position(pos)
            , button(button)
            , scrollWheel(0.0f)
        {
        }

        PointerEventArgs(const InterfacesVector &pos, const InterfacesVector &move, float scroll = 0.0f,
            MouseButton::MouseButton button = MouseButton::NO_BUTTON)
            : position(pos)
            , moveDelta(move)
            , button(button)
            , scrollWheel(scroll)
        {
        }

        InterfacesVector position, moveDelta;
        MouseButton::MouseButton button;
        float scrollWheel;
    };
}
}
