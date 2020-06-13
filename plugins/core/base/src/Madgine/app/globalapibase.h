#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/keyvalue/virtualscopebase.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine {
namespace App {
    struct MADGINE_BASE_EXPORT GlobalAPIBase : MadgineObject, VirtualScopeBase {
        GlobalAPIBase(App::Application &app);

        template <typename T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        GlobalAPIBase &getSelf(bool = true);

        virtual const MadgineObject *parent() const override;

    protected:
        bool init() override;
        void finalize() override;

        App::Application &mApp;
    };
}
}

RegisterType(Engine::App::GlobalAPIBase);