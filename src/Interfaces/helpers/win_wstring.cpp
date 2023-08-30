#include "win_wstring.h"

#define NOMINMAX
#include <Windows.h>

namespace Engine {
namespace StringUtil {

    std::wstring toWString(std::string_view input)
    {
        return { input.begin(), input.end() };
    }

    std::string fromWString(std::wstring_view input)
    {
        return { input.begin(), input.end() };
    }

}
}