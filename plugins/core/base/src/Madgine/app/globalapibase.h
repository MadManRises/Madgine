#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalueutil/virtualscope.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace App {
    struct MADGINE_BASE_EXPORT GlobalAPIBase : VirtualScopeBase, MadgineObject<GlobalAPIBase> {
        GlobalAPIBase(App::Application &app);

        template <typename T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        const App::Application *parent() const;
        App::Application &app();

    protected:
        virtual bool init();
        virtual void finalize();

        friend struct MadgineObject<GlobalAPIBase>;

        App::Application &mApp;
    };
}
}

RegisterType(Engine::App::GlobalAPIBase);