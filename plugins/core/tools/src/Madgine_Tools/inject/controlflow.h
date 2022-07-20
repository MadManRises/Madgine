#pragma once

namespace Engine {
namespace Inject {

    struct ControlFlow {

        struct BasicBlock {
            BasicBlock(void *startAddress)
                : mAddresses({ startAddress })
            {
            }

            BasicBlock(std::vector<void *> addresses, std::set<size_t> targets)
                : mAddresses(std::move(addresses))
                , mTargets(std::move(targets))
            {
            }

            std::vector<void *> mAddresses;
            std::set<size_t> mTargets;
        };

        using Graph = std::vector<BasicBlock>;

        struct Context {
            Context();

            void notify();
            void notify(size_t skip);

            const Graph &graph() const;

        private:
            Graph mGraph;
            size_t mCurrentBlock = 0;
        };
    };

}
}