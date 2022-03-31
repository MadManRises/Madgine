#pragma once

#include "madgineobject/madgineobject.h"

#include "globalapicollector.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Modules/threading/taskqueue.h"

namespace Engine {
namespace App {

    /**
     * @brief The Application manages all GlobalAPIComponents
     * 
     * The Application creates all registered GlobalAPIComponents.
     * It also creates a TaskQueue for game logic tasks.
    */
    struct MADGINE_BASE_EXPORT Application : MadgineObject<Application> {
        Application();
        ~Application();

        template <typename T>
        T &getGlobalAPIComponent()
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>()));
        }     

        GlobalAPIBase &getGlobalAPIComponent(size_t i);        

        Threading::TaskQueue *taskQueue();

        static Application &getSingleton();
        static Application *getSingletonPtr();

    protected:
        Threading::Task<bool> init();
        Threading::Task<void> finalize();
        friend struct MadgineObject<Application>;

    private:
        Threading::TaskQueue mTaskQueue;

    public:
        GlobalAPIContainer<std::vector<Placeholder<0>>> mGlobalAPIs;
    };
}
}

RegisterType(Engine::App::Application);
