#pragma once

#include "Interfaces/window/windoweventlistener.h"

#include "../render/rendercontextcollector.h"

#include "mainwindowcomponentcollector.h"

#include "madgineobject/madgineobject.h"

#include "Meta/serialize/serializabledataunit.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/threading/taskqueue.h"

namespace Engine {
namespace Window {

    struct MADGINE_CLIENT_EXPORT MainWindowComponentComparator {

        bool operator()(const std::unique_ptr<MainWindowComponentBase> &first, const std::unique_ptr<MainWindowComponentBase> &second) const;

        struct traits {
            using type = int;
            using cmp_type = MainWindowComponentComparator;
            using item_type = std::unique_ptr<MainWindowComponentBase>;

            static int to_cmp_type(const item_type &value);
        };
    };

    /**
     * @brief The MainWindow manages multiple concepts relevant for client applications.
     * 
     * The responsibilities of the class include:
     *  - creation of an OSWindow   
     *  - creation of a RenderContext and a RenderTarget for the OSWindow
     *  - creation of a TaskQueue for the render thread
     *  - creation & management of all MainWindowComponents
     * 
     * The MainWindowComponents are ordered by priority. The order is relevant for input-event
     * propagation and render order.
     *      
    */
    struct MADGINE_CLIENT_EXPORT MainWindow : WindowEventListener,
                                              Serialize::SerializableDataUnit,
                                              MadgineObject<MainWindow> {
        SERIALIZABLEUNIT(MainWindow);

        MainWindow(const WindowSettings &settings);
        ~MainWindow();

        Threading::Task<bool> init();
        Threading::Task<void> finalize();

        void render();

        auto &components()
        {
            return mComponents;
        }

        template <typename T>
        T &getWindowComponent()
        {
            return static_cast<T &>(getWindowComponent(component_index<T>()));
        }

        MainWindowComponentBase &getWindowComponent(size_t i);

        ToolWindow *createToolWindow(const WindowSettings &settings);
        void destroyToolWindow(ToolWindow *w);

        OSWindow *osWindow() const;

        Render::RenderContext *getRenderer();
        Render::RenderTarget *getRenderWindow();

        Threading::TaskQueue *taskQueue();
        void shutdown();

        Rect2i getScreenSpace();
        void applyClientSpaceResize(MainWindowComponentBase *component = nullptr);

        bool injectKeyPress(const Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Input::PointerEventArgs &arg) override;
        bool injectAxisEvent(const Input::AxisEventArgs &arg) override;

    protected:
        void onClose() override;
        void onRepaint() override;
        void onResize(const InterfacesVector &size) override;

        void storeWindowData();

    private:
        const WindowSettings &mSettings;

        Threading::TaskQueue mTaskQueue;

        MainWindowComponentContainer<std::set<Placeholder<0>, MainWindowComponentComparator>> mComponents;

        std::list<ToolWindow> mToolWindows;

        OSWindow *mOsWindow = nullptr;
        std::optional<Render::RenderContextSelector> mRenderContext;
        std::unique_ptr<Render::RenderTarget> mRenderWindow;
    };

}
}
