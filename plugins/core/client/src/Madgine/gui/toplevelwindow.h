#pragma once

#include "../input/inputcollector.h"
#include "../input/inputlistener.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/generic/transformIt.h"


#include "Modules/serialize/toplevelserializableunit.h"
#include "Modules/keyvalue/observablecontainer.h"
#include "Modules/madgineobject/madgineobjectobserver.h"

#include "Modules/serialize/container/controlledcontainer.h"

#include "../render/rendercontextcollector.h"

#include "../threading/frameloop.h"

#include "Modules/math/vector3i.h"

#include "toplevelwindowcomponent.h"


namespace Engine {
namespace GUI {

    class MADGINE_CLIENT_EXPORT TopLevelWindow : public ScopeBase,
                                                 public Input::InputListener,
                                                 public Window::WindowEventListener,
                                                 public Threading::FrameListener,
                                                 public Serialize::TopLevelSerializableUnit<TopLevelWindow>,
                                                 public MadgineObject {
        SERIALIZABLEUNIT;

    public:
        TopLevelWindow(const Window::WindowSettings &settings);
        virtual ~TopLevelWindow();


        Rect2i getScreenSpace();

        Input::InputHandler *input();

        bool injectKeyPress(const Input::KeyEventArgs &arg) override;
        bool injectKeyRelease(const Input::KeyEventArgs &arg) override;
        bool injectPointerPress(const Input::PointerEventArgs &arg) override;
        bool injectPointerRelease(const Input::PointerEventArgs &arg) override;
        bool injectPointerMove(const Input::PointerEventArgs &arg) override;

        Window::Window *window() const;


        decltype(auto) components()
        {
            return uniquePtrToPtr(mComponents);
        }

        Render::RenderContext *getRenderer();
        Render::RenderTarget *getRenderWindow();

        ToolWindow *createToolWindow(const Window::WindowSettings &settings);
        void destroyToolWindow(ToolWindow *w);

        virtual bool frameStarted(std::chrono::microseconds) override;
        virtual bool frameRenderingQueued(std::chrono::microseconds, Threading::ContextMask) override;
        virtual bool frameEnded(std::chrono::microseconds) override;



        template <class T>
        T &getWindowComponent(bool init = true)
        {
            return static_cast<T &>(getWindowComponent(component_index<T>(), init));
        }

        TopLevelWindowComponentBase &getWindowComponent(size_t i, bool = true);


        virtual const MadgineObject *parent() const override;
        virtual bool init() override;
        virtual void finalize() override;

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

		void shutdown();

        Threading::FrameLoop &frameLoop();

		void applyClientSpaceResize(TopLevelWindowComponentBase *component = nullptr);

    protected:
        void onClose() override;
        void onRepaint() override;
        void onResize(size_t width, size_t height) override;


    private:
        Window::Window *mWindow = nullptr;
        std::optional<Render::RenderContextSelector> mRenderContext;
        std::unique_ptr<Render::RenderTarget> mRenderWindow;

        Input::InputHandler *mExternalInput = nullptr;
        std::optional<Input::InputHandlerSelector> mInputHandlerSelector;

        std::vector<std::unique_ptr<ToolWindow>> mToolWindows;

        SERIALIZABLE_CONTAINER_EXT(mComponents, TopLevelWindowContainer < PartialObservableContainer<elevate<, Serialize::ControlledContainer, , partial<std::set, type_pack<>, type_pack<TopLevelWindowComponentComparator>>::type>::type, MadgineObjectObserver>::type>);

        Threading::FrameLoop mLoop;

        const Window::WindowSettings &mSettings;
    };

}
}
