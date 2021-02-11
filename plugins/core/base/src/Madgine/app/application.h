#pragma once

#include "madgineobject/madgineobject.h"

#include "globalapicollector.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Modules/threading/defaulttaskqueue.h"

namespace Engine {
namespace App {

    struct MADGINE_BASE_EXPORT Application : MadgineObject<Application> {
        Application(const AppSettings &settings);
        ~Application();

        template <typename T>
        T &getGlobalAPIComponent(bool init = true)
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t, bool = true);

        const AppSettings &settings();

        static Application &getSingleton();

    protected:
        bool init();
        void finalize();
        friend struct MadgineObject<Application>;

    private:
        const AppSettings &mSettings;

        int mGlobalAPIInitCounter = 0;

        Threading::DefaultTaskQueue mTaskQueue;

    public:
        GlobalAPIContainer<std::vector<Placeholder<0>>> mGlobalAPIs;
    };
}
}

RegisterType(Engine::App::Application);
