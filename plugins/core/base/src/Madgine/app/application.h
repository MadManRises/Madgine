#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "globalapicollector.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Modules/threading/defaulttaskqueue.h"

namespace Engine {
namespace App {

    struct MADGINE_BASE_EXPORT Application : ScopeBase,
                                             MadgineObject {
        Application(const AppSettings &settings);

        virtual ~Application();

        template <typename T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t, bool = true);

        Application &getSelf(bool = true);

        const AppSettings &settings();

        virtual const MadgineObject *parent() const override;

        static Application &getSingleton();

    protected:
        bool init() override;

        void finalize() override;

    private:
        const AppSettings &mSettings;

        int mGlobalAPIInitCounter;

        Threading::DefaultTaskQueue mTaskQueue;

    public:
        GlobalAPIContainer<std::vector<Placeholder<0>>> mGlobalAPIs;
    };
}
}

RegisterType(Engine::App::Application);
