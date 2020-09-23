#include "../uilib.h"
#include "handler.h"
#include "Madgine/widgets/widget.h"
#include "Madgine/window/mainwindow.h"
#include "uimanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::UI::Handler)
PROPERTY(Widget, widget, setWidget)
METATABLE_END(Engine::UI::Handler)

SERIALIZETABLE_BEGIN(Engine::UI::Handler)
ENCAPSULATED_POINTER(mWidget, widget, setWidget)
SERIALIZETABLE_END(Engine::UI::Handler)

namespace Engine {
namespace UI {
    Handler::Handler(UIManager &ui)
        : mUI(ui)
        , mPointerMoveSlot(this)
        , mPointerDownSlot(this)
        , mPointerUpSlot(this)
    {
    }

    UI::GuiHandlerBase &Handler::getGuiHandler(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return mUI.getGuiHandler(i, init);
    }

    GameHandlerBase &Handler::getGameHandler(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return mUI.getGameHandler(i, init);
    }

    void Handler::sizeChanged()
    {
    }

    /*App::Application& Handler::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mUI.app(init);
		}*/

    const UIManager *Handler::parent() const
    {
        return &mUI;
    }

    /*UIManager &Handler::ui(bool init)
    {
        if (init) {
            checkInitState();
        }
        return mUI.getSelf(init);
    }*/

    bool Handler::init()
    {
        return true;
    }

    void Handler::finalize()
    {
        mWidget = nullptr;
        for (const WindowDescriber &des : mWidgets) {
            des.mInit(nullptr);
        }
    }

    void Handler::setWidget(Widgets::WidgetBase *widget)
    {
        if (mWidget != widget) {
            if (mWidget) {
                mPointerMoveSlot.disconnectAll();
                mPointerDownSlot.disconnectAll();
                mPointerUpSlot.disconnectAll();
            }
            mWidget = widget;

            if (mWidget) {
                mWidget->pointerMoveEvent().connect(mPointerMoveSlot);
                mWidget->pointerDownEvent().connect(mPointerDownSlot);
                mWidget->pointerUpEvent().connect(mPointerUpSlot);

                for (const WindowDescriber &des : mWidgets) {
                    Widgets::WidgetBase *child = widget->getChildRecursive(des.mWidgetName);

                    if (!child) {
                        LOG_ERROR("Window not found: \"" << des.mWidgetName << "\"");
                    }
                    if (!des.mInit(child))
                        LOG_ERROR("ERROR");
                }
            } else {
                for (const WindowDescriber &des : mWidgets) {                    
                    if (!des.mInit(nullptr))
                        LOG_ERROR("ERROR");
                }
            }
        }
    }

    void Handler::injectPointerMove(const Input::PointerEventArgs &evt)
    {
        onPointerMove(evt);
    }

    void Handler::injectPointerDown(const Input::PointerEventArgs &evt)
    {
        onPointerDown(evt);
    }

    void Handler::injectPointerUp(const Input::PointerEventArgs &evt)
    {
        onPointerUp(evt);
    }

    bool Handler::injectKeyPress(const Input::KeyEventArgs &evt)
    {
        return onKeyPress(evt);
    }

    void Handler::onPointerMove(const Input::PointerEventArgs &me)
    {
    }

    void Handler::onPointerDown(const Input::PointerEventArgs &me)
    {
    }

    void Handler::onPointerUp(const Input::PointerEventArgs &me)
    {
    }

    bool Handler::onKeyPress(const Input::KeyEventArgs &evt)
    {
        return false;
    }

    void Handler::registerWidget(const std::string &name, std::function<bool(Widgets::WidgetBase *)> init)
    {
        assert(!mWidget);
        mWidgets.emplace_back(name, init);
    }

    void Handler::onMouseVisibilityChanged(bool b)
    {
    }

    Widgets::WidgetBase *Handler::widget() const
    {
        return mWidget;
    }

}
}
