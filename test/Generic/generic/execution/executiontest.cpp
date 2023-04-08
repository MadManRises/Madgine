#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Generic/execution/algorithm.h"
#include "Generic/execution/promise.h"

#include "Generic/execution/state.h"
#include "Generic/genericresult.h"

TEST(Execution, Basic)
{
    using namespace Engine::Execution;

    std::vector<int> cont { 1, 2, 3, 4 };
    int v = 12;

    auto inner = [](int i, int a) {
        return just(int { i }, int { a })
            | then([](const auto &...v) { return (v + ...); })
            | then([](int i) { std::cout << i << std::endl; });
    };

    auto graph = [&](std::vector<int> &cont) {
        return sequence(
            for_each(cont,
                [&](int i) {
                    return Engine::Execution::read<"test">() | let_value([&, i](int a) {
                        return inner(i, a);
                    });
                }),
            Variable<"test">(12));
    };

    //detach(graph(cont));
    detach(sequence(Engine::Execution::read<"test">(), Variable<"test">(12)));
}
