#pragma once

#include "madgineobject/madgineobject.h"

#include "globalapicollector.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Modules/threading/taskqueue.h"

namespace Engine {
namespace App {

    struct MADGINE_BASE_EXPORT Application : MadgineObject<Application> {
        Application(const AppSettings &settings);
        ~Application();

        template <typename T>
        T &getGlobalAPIComponent()
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>()));
        }     

        GlobalAPIBase &getGlobalAPIComponent(size_t i);

        const AppSettings &settings();

        static Application &getSingleton();

        Threading::TaskQueue *taskQueue();

    protected:
        Threading::Task<bool> init();
        Threading::Task<void> finalize();
        friend struct MadgineObject<Application>;

    private:
        const AppSettings &mSettings;

        Threading::TaskQueue mTaskQueue;

    public:
        GlobalAPIContainer<std::vector<Placeholder<0>>> mGlobalAPIs;
    };
}
}

RegisterType(Engine::App::Application);
