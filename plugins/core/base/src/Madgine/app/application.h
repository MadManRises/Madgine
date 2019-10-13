#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "globalapicollector.h"

#include "Modules/threading/scheduler.h"

namespace Engine {
namespace App {
    /**
		 * \brief The Base-class for any Application that runs the Madgine.
		 */
    class MADGINE_BASE_EXPORT Application : public ScopeBase,
                                            public MadgineObject {
    public:

        /**
			 * \brief Creates the Application
			 * \param state A pointer to the global LuaState to which this application will be registered.
			 */
        Application(const AppSettings &settings);

        /**
			 * \brief Deletes all objects created by the Application.
			 */
        virtual ~Application();

        /**
			* Marks the Application as shutdown. This causes the event loop to return within the next frame.
			*/
        //void shutdown();

        /**
			 * \brief Tells if the application is currently running.
			 * \return <code>true</code>, if the application is shutdown, so not running. <code>false</code>, otherwise.
			 */
        //bool isShutdown() const;

        /**
			* \brief Retrieve the statistical frames-per-second value.
			* \return the current frames-per-second.
			*/
        float getFPS();

        //KeyValueMapList maps() override;

        template <class T>
        T &getGlobalAPIComponent()
        {
            return static_cast<T &>(getGlobalAPIComponent(component_index<T>()));
        }

        GlobalAPIBase &getGlobalAPIComponent(size_t, bool = true);

        template <class T>
        T &getSceneComponent()
        {
            return static_cast<T &>(getSceneComponent(component_index<T>()));
        }

        Scene::SceneComponentBase &getSceneComponent(size_t, bool = true);
        Scene::SceneManager &sceneMgr(bool = true);
        Application &getSelf(bool = true);

        
        const AppSettings &settings();

        Debug::Profiler::Profiler &profiler();

        virtual const MadgineObject *parent() const override;
        
		static Application &getSingleton();

    protected:

        bool init() override;

        void finalize() override;

    private:
        const AppSettings &mSettings;

        std::unique_ptr<Debug::Profiler::Profiler> mProfiler;

        int mGlobalAPIInitCounter;

    public:
        GlobalAPIContainer<std::vector> mGlobalAPIs;
    };
}
}
