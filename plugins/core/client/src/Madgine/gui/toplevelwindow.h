#pragma once

#include "../input/inputcollector.h"
#include "../input/inputlistener.h"
#include "Interfaces/window/windoweventlistener.h"
#include "Modules/uniquecomponent/uniquecomponentselector.h"

#include "Modules/generic/transformIt.h"

#include "Modules/madgineobject/madgineobjectobserver.h"
#include "Modules/serialize/toplevelserializableunit.h"

#include "Modules/serialize/container/controlledcontainer.h"

#include "../render/rendercontextcollector.h"

#include "../threading/frameloop.h"

#include "Modules/math/vector3i.h"

#include "toplevelwindowcomponentcollector.h"

#include "Modules/keyvalue/keyvalueset.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/madgineobject/madgineobject.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

namespace Engine {
namespace GUI {

    template <typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct TopLevelWindowComponentObserver : MadgineObjectObserver {
        template <typename It>
        void operator()(const It &it, int event)
        {
            MadgineObjectObserver::operator()(it, event);

            if (event == (AFTER | ACTIVATE_ITEM)) {
                auto end = OffsetPtr::parent(this)->mComponents.end();
                auto next = std::next(it);
                if (end == next)
                    OffsetPtr::parent(this)->applyClientSpaceResize();
                else
                    OffsetPtr::parent(this)->applyClientSpaceResize(next->get());
            }
            if (event == (AFTER | DEACTIVATE_ITEM)) {
                auto end = OffsetPtr::parent(this)->mComponents.end();
                if (end == it)
                    OffsetPtr::parent(this)->applyClientSpaceResize();
                else
                    OffsetPtr::parent(this)->applyClientSpaceResize(it->get());
            }
        }
    };

    struct MADGINE_CLIENT_EXPORT TopLevelWindowComponentComparator {
        using cmp_type = int;

        bool operator()(const std::unique_ptr<TopLevelWindowComponentBase> &first, const std::unique_ptr<TopLevelWindowComponentBase> &second) const
        {
            return first->mPriority < second->mPriority;
        }
    };

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

        OFFSET_CONTAINER(mComponents, TopLevelWindowComponentContainer<Serialize::ControlledContainer<KeyValueSet<Placeholder<0>, TopLevelWindowComponentComparator>, TopLevelWindowComponentObserver<>>>);

        Threading::FrameLoop mLoop;

        const Window::WindowSettings &mSettings;

        template <typename OffsetPtr>
        friend struct TopLevelWindowComponentObserver;
    };

}
}
