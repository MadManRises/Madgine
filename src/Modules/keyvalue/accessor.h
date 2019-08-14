#pragma once

namespace Engine {


struct Accessor {
    ValueType (*mGetter)(TypedScopePtr);
    void (*mSetter)(TypedScopePtr, ValueType);
};

}