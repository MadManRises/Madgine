#include "../moduleslib.h"

#if ENABLE_THREADING

#include "threadlocal.h"

#if USE_PTHREAD_THREADLOCAL_STORE
#include <pthread.h>
#endif

namespace Engine {
namespace Threading {

    static std::vector<std::function<Any()>> &sLocalBssVariableConstructors()
    {
        static std::vector<std::function<Any()>> dummy;
        return dummy;
    }

    static std::vector<std::function<Any()>> &sLocalObjectVariableConstructors()
    {
        static std::vector<std::function<Any()>> dummy;
        return dummy;
    }

    struct VariableStore {
        VariableStore()
        {
            for (const std::function<Any()> &ctor : sLocalBssVariableConstructors()) {
                mBssVariables.emplace_back(ctor());
            }
        }

        void init()
        {
            while (mObjectVariables.size() < sLocalObjectVariableConstructors().size()) {
                mObjectVariables.emplace_back(sLocalObjectVariableConstructors()[mObjectVariables.size()]());
            }
        }

        std::vector<Any> mBssVariables;
        std::vector<Any> mObjectVariables;
    };

#if USE_PTHREAD_THREADLOCAL_STORE
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
#else
    static VariableStore &sLocalVariables()
    {
        static thread_local VariableStore dummy;
        return dummy;
    }
#endif

    int ThreadLocalStorage::registerLocalBssVariable(std::function<Any()> ctor)
    {
        sLocalBssVariableConstructors().emplace_back(std::move(ctor));
        return -static_cast<int>(sLocalBssVariableConstructors().size());
    }

    void ThreadLocalStorage::unregisterLocalBssVariable(int index)
    {
        //sLocalVariables().mBssVariables[-(index + 1)] = {};
        sLocalBssVariableConstructors()[-(index + 1)] = {};
    }

    int ThreadLocalStorage::registerLocalObjectVariable(std::function<Any()> ctor)
    {
        sLocalObjectVariableConstructors().emplace_back(std::move(ctor));
        return sLocalObjectVariableConstructors().size() - 1;
    }

    void ThreadLocalStorage::unregisterLocalObjectVariable(int index)
    {
        //sLocalVariables().mObjectVariables[index] = {};
        sLocalObjectVariableConstructors()[index] = {};
    }

    const Any &ThreadLocalStorage::localVariable(int index)
    {
        std::vector<Any> &variables = index < 0 ? sLocalVariables().mBssVariables : sLocalVariables().mObjectVariables;
        std::vector<std::function<Any()>> &constructors = index < 0 ? sLocalBssVariableConstructors() : sLocalObjectVariableConstructors();
        if (index < 0)
            index = -(index + 1);
        while (variables.size() <= index) {
            variables.emplace_back(constructors[variables.size()]());
        }
        return variables.at(index);
    }

    void ThreadLocalStorage::init(bool bss)
    {
        if (bss) {

#if USE_PTHREAD_THREADLOCAL_STORE
            static int result = pthread_key_create(&sKey(), &destructTLS);
            assert(result == 0);

            pthread_setspecific(sKey(), new VariableStore);
#endif

            while (sLocalVariables().mBssVariables.size() < sLocalBssVariableConstructors().size()) {
                sLocalVariables().mBssVariables.emplace_back(sLocalBssVariableConstructors()[sLocalVariables().mBssVariables.size()]());
            }
        } else {
            sLocalVariables().init();
        }
    }

    void ThreadLocalStorage::finalize(bool bss)
    {
        if (bss) {
            sLocalVariables().mBssVariables.clear();
        } else {
            sLocalVariables().mObjectVariables.clear();
        }
    }

}
}

#endif