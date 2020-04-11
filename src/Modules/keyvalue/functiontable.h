#pragma once

namespace Engine {

struct FunctionTable {
    size_t mArgumentsCount = 0;
};

}

DLL_IMPORT_VARIABLE2(const Engine::FunctionTable, function, auto F);