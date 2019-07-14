#pragma once

namespace Engine {


struct Accessor {
    ValueType (*mGetter)(ScopeBase *);
    void (*mSetter)(ScopeBase *, ValueType);
};

}