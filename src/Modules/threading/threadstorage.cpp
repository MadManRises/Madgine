#include "../moduleslib.h"

#if ENABLE_THREADING

#    include "threadstorage.h"
#include "../generic/any.h"

#    if USE_PTHREAD_THREADLOCAL_STORE
#        include <pthread.h>
#    endif

namespace Engine {
namespace Threading {

    static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &sLocalBssVariableConstructors()
    {
        static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> dummy;
        return dummy;
    }

    static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &sLocalObjectVariableConstructors()
    {
        static std::vector<std::pair<std::function<Any()>, std::vector<Any>>> dummy;
        return dummy;
    }

    struct VariableStore {
        VariableStore()
        {
            mIndex = sCount.fetch_add(1);

            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sLocalBssVariableConstructors()) {
                if (p.second.size() <= mIndex)
                    p.second.resize(mIndex + 1);
                if (!p.second[mIndex])
                    p.second[mIndex] = p.first();
            }
        }

        void init()
        {
            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sLocalObjectVariableConstructors()) {
                if (p.second.size() <= mIndex)
                    p.second.resize(mIndex + 1);
                if (!p.second[mIndex])
                    p.second[mIndex] = p.first();
            }
        }

        size_t mIndex;
        static inline std::atomic<size_t> sCount = 0;
    };

#    if USE_PTHREAD_THREADLOCAL_STORE
    static pthread_key_t &sKey()
    {
        static pthread_key_t dummy;
        return dummy;
    }

    static VariableStore &sLocalVariables()
    {
        VariableStore *store = static_cast<VariableStore *>(pthread_getspecific(sKey()));
        assert(store);
        return *store;
    }

    static void destructTLS(void *store)
    {
        delete static_cast<VariableStore *>(store);
    }
#    else
    static VariableStore &sLocalVariables()
    {
        static thread_local VariableStore dummy;
        return dummy;
    }
#    endif

    int ThreadStorage::registerLocalBssVariable(std::function<Any()> ctor)
    {
        sLocalBssVariableConstructors().emplace_back(std::move(ctor), std::vector<Any> {});
        return -static_cast<int>(sLocalBssVariableConstructors().size());
    }

    void ThreadStorage::unregisterLocalBssVariable(int index)
    {
        sLocalBssVariableConstructors()[-(index + 1)] = {};
    }

    int ThreadStorage::registerLocalObjectVariable(std::function<Any()> ctor)
    {
        sLocalObjectVariableConstructors().emplace_back(std::move(ctor), std::vector<Any> {});
        return sLocalObjectVariableConstructors().size() - 1;
    }

    void ThreadStorage::unregisterLocalObjectVariable(int index)
    {
        sLocalObjectVariableConstructors()[index] = {};
    }

    const Any &ThreadStorage::localVariable(int index)
    {
        size_t self = sLocalVariables().mIndex;
        std::vector<std::pair<std::function<Any()>, std::vector<Any>>> &constructors = index < 0 ? sLocalBssVariableConstructors() : sLocalObjectVariableConstructors();
        if (index < 0)
            index = -(index + 1);
        std::pair<std::function<Any()>, std::vector<Any>> &p = constructors[index];
        if (p.second.size() <= self)
            p.second.resize(self + 1);
        if (!p.second[self])
            p.second[self] = p.first();
        return p.second.at(self);
    }

    void ThreadStorage::init(bool bss)
    {
        if (bss) {

#    if USE_PTHREAD_THREADLOCAL_STORE
            static int result = pthread_key_create(&sKey(), &destructTLS);
            assert(result == 0);

            pthread_setspecific(sKey(), new VariableStore);
#    endif

            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sLocalBssVariableConstructors()) {
                if (p.second.size() <= sLocalVariables().mIndex)
                    p.second.resize(sLocalVariables().mIndex + 1);
                if (!p.second[sLocalVariables().mIndex])
                    p.second[sLocalVariables().mIndex] = p.first();
            }
        } else {
            sLocalVariables().init();
        }
    }

    void ThreadStorage::finalize(bool bss)
    {
        if (bss) {
            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sLocalBssVariableConstructors()) {
                if (p.second.size() > sLocalVariables().mIndex)
                    p.second[sLocalVariables().mIndex] = {};
            }
        } else {
            for (std::pair<std::function<Any()>, std::vector<Any>> &p : sLocalObjectVariableConstructors()) {
                if (p.second.size() > sLocalVariables().mIndex)
                    p.second[sLocalVariables().mIndex] = {};
            }
        }
    }

}
}

#endif