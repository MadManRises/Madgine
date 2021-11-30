#pragma once

#include "madgineobject/madgineobject.h"

#include "Meta/keyvalue/virtualscope.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace App {
    struct MADGINE_BASE_EXPORT GlobalAPIBase : VirtualScopeBase<>, MadgineObject<GlobalAPIBase> {
        GlobalAPIBase(App::Application &app);
        virtual ~GlobalAPIBase() = default;

        template <typename T>
        T &getGlobalAPIComponent()
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>()));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t i);

        App::Application &app();

        Threading::TaskQueue *taskQueue() const;

    protected:
        virtual Threading::Task<bool> init();
        virtual Threading::Task<void> finalize();

        friend struct MadgineObject<GlobalAPIBase>;

        App::Application &mApp;
    };
}
}

RegisterType(Engine::App::GlobalAPIBase);