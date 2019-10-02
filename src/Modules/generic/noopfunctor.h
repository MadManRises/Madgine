#pragma once

namespace Engine {

struct NoOpFunctor {
    /*template <typename... Args>
    void operator()(Args &&...) {}*/
    void operator()() {}
};

}