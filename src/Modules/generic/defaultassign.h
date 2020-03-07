#pragma once

namespace Engine {

struct DefaultAssign {
    template <typename T, typename V>
    void operator()(T &t, V &&v)
    {
        t = std::forward<V>(v);
    }
};

}