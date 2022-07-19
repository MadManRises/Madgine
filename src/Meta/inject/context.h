#pragma once

#include "../../Generic/derive.h"

namespace Engine {
namespace Inject {

    DERIVE_FUNCTION(setup)

    struct Context;

    struct noContext {
    };

    static thread_local std::weak_ptr<Context> sGenericContext;

        struct Context {
        virtual ~Context()
        {
            sGenericContext = mStoredContext;
        }

        void setup(std::shared_ptr<Context> store)
        {
            mStoredContext = std::exchange(sGenericContext, store);
        }

        std::weak_ptr<Context> mStoredContext;
    };

    
    template <typename... Injectors>
    struct ContextStore : Context {
    };

    template <typename... Injectors>
    static thread_local std::weak_ptr<ContextStore<Injectors...>> sContext;


    template <typename Injector, typename... Injectors>
    struct ContextStore<Injector, Injectors...> : ContextStore<Injectors...> {
        ContextStore()
        {
        }

        ~ContextStore()
        {
            sContext<Injector> = mStoredContext;
        }

        void setup(std::shared_ptr<Context> store)
        {
            mStoredContext = std::exchange(sContext<Injector, Injectors...>, std::static_pointer_cast<ContextStore<Injector, Injectors...>>(store));
            ContextStore<Injectors...>::setup(store);
            if constexpr (has_function_setup<typename Injector::Context>())
                mContext.setup();
        }

        std::weak_ptr<ContextStore<Injector, Injectors...>> mStoredContext;
        typename Injector::Context mContext;
    };

    template <typename... Injectors>
    std::shared_ptr<ContextStore<Injectors...>> make_context()
    {
        if (std::shared_ptr<Context> ptr = sGenericContext.lock()) {
            if (dynamic_cast<ContextStore<Injectors...> *>(ptr.get()))
                return static_pointer_cast<ContextStore<Injectors...>>(ptr);
        }

        std::shared_ptr<ContextStore<Injectors...>> c = std::make_shared<ContextStore<Injectors...>>();

        c->setup(c);

        return c;
    }

    template <typename Injector, typename... Injectors>
    std::shared_ptr<typename Injector::Context> get_context(std::shared_ptr<Context> ptr) {
        return { ptr, &dynamic_cast<ContextStore<Injector, Injectors...> *>(ptr.get())->mContext };
    }

}
}