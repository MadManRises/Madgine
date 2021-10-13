#include "../toolslib.h"
#include "Interfaces/interfaceslib.h"

#include "controlflow.h"

#include "Interfaces/debug/stacktrace.h"

namespace Engine {
namespace Inject {

    ControlFlow::Context::Context()
    {
        Debug::StackTrace<20> trace = Debug::StackTrace<20>::getCurrent(7);

        mGraph.emplace_back(*trace.begin());
        mCurrentBlock = 0;
    }

    void ControlFlow::Context::notify()
    {
        notify(3);
    }

    void ControlFlow::Context::notify(size_t skip)
    {
        Debug::StackTrace<20> trace = Debug::StackTrace<20>::getCurrent(skip + 1);

        void *address = *trace.begin();

        BasicBlock &block = mGraph[mCurrentBlock];

        if (address > block.mAddresses.back()) {
            block.mAddresses.push_back(address);
        } else {

            for (size_t i = 0; i < mGraph.size(); ++i) {
                BasicBlock &b = mGraph[i];
                if (b.mAddresses.front() <= address && address <= b.mAddresses.back()) {

                    if (address == b.mAddresses.front()) {
                        block.mTargets.insert(i);
                        return;
                    } else {
                        for (auto it = b.mAddresses.begin(); it != b.mAddresses.end(); ++it) {
                            void *a = *it;
                            if (address <= a) {
                                std::vector<void *> addresses { it, b.mAddresses.end() };
                                std::set<size_t> targets { std::move(b.mTargets) };

                                b.mTargets.clear();
                                b.mAddresses.erase(it, b.mAddresses.end());
                                b.mTargets.insert(mGraph.size() - 1);
                                
                                mGraph.emplace_back(std::move(addresses), std::move(targets));

                                if (address != a) {
                                    mGraph.emplace_back(address);
                                }

                                mGraph[mCurrentBlock].mTargets.insert(mGraph.size() - 1);
                                mCurrentBlock = mGraph.size() - 1;

                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    const ControlFlow::Graph &ControlFlow::Context::graph() const
    {
        return mGraph;
    }

}
}