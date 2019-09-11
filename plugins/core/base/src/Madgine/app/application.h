#pragma once

#include "Modules/madgineobject/madgineobject.h"

#include "globalapicollector.h"

#include "../threading/framelistener.h"

#include "../threading/frameloop.h"
#include "Modules/threading/scheduler.h"

namespace Engine {
namespace App {
    /**
		 * \brief The Base-class for any Application that runs the Madgine.
		 */
    class MADGINE_BASE_EXPORT Application : public ScopeBase,
                                            public MadgineObject,
                                            public Threading::FrameListener {
    public:
        /**
			* Convenience method, that creates the Application of type T, calls setup(), init() and go() with the given <code>settings</code> and returns the result of the call to go().
			*
			* @return result of the call to go()
			* @param settings the settings for the Application
			*/
        static int run(AppSettings &settings, Threading::WorkGroup &workGroup)
        {
            Application app(settings);
            return Threading::Scheduler(workGroup, { &app.frameLoop() }).go();
        }

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
        virtual void shutdown();

        /**
			* This will be called once every frame. It returns <code>false</code>, if the Application was shutdown().
			* Otherwise it will call frameRenderingQueued, perform a fixedUpdate if necessary and update all components that need a regular update:
			*  - The ConnectionManager
			*  - All GlobalApiComponents
			*  
			* This is the toplevel method of the Madgine, in which every derived class should recursively update all elements that need update per frame.
			*
			* @return <code>true</code>, if the Application is not shutdown, <code>false</code> otherwise
			* @param timeSinceLastFrame holds the time since the last frame
			*/
        virtual bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context) override;

        /**
			 * \brief Tells if the application is currently running.
			 * \return <code>true</code>, if the application is shutdown, so not running. <code>false</code>, otherwise.
			 */
        bool isShutdown() const;

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

        /**
			* \brief Adds a FrameListener to the application.
			* \param listener the FrameListener to be added.
			*/
        void addFrameListener(Threading::FrameListener *listener);
        /**
			* \brief Removes a FrameListener from the application.
			* \param listener the FrameListener to be removed.
			*/
        void removeFrameListener(Threading::FrameListener *listener);

        void singleFrame();

        Threading::FrameLoop &frameLoop();

        const AppSettings &settings();

        Debug::Profiler::Profiler &profiler();

        virtual const MadgineObject *parent() const override;
        
		static Application &getSingleton();

    protected:
        virtual void clear();

        bool init() override;

        void finalize() override;

    private:
        const AppSettings &mSettings;

        std::unique_ptr<Debug::Profiler::Profiler> mProfiler;

        Threading::FrameLoop mLoop;

        int mGlobalAPIInitCounter;

    public:
        GlobalAPIContainer<std::vector> mGlobalAPIs;
    };
}
}
