#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "Modules/keyvalue/scopebase.h"

namespace Engine {
namespace App {
    class MADGINE_BASE_EXPORT GlobalAPIBase : public MadgineObject, public ScopeBase {
    public:
        GlobalAPIBase(App::Application &app);

        template <class T>
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