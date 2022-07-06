#pragma once

#include "Modules/threading/madgineobject.h"

#include "Meta/keyvalue/virtualscope.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace Base {
    struct MADGINE_BASE_EXPORT GlobalAPIBase : VirtualScopeBase<>, Threading::MadgineObject<GlobalAPIBase> {
        GlobalAPIBase(Application &app);
        virtual ~GlobalAPIBase() = default;

        template <typename T>
        T &getGlobalAPIComponent()
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>()));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t i);

        Application &app();

        Threading::TaskQueue *taskQueue() const;

    protected:
        virtual Threading::Task<bool> init();
        virtual Threading::Task<void> finalize();

        friend struct MadgineObject<GlobalAPIBase>;

        Application &mApp;
    };
}
}

REGISTER_TYPE(Engine::Base::GlobalAPIBase)