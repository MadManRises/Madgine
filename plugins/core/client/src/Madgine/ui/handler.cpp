#include "../clientlib.h"
#include "handler.h"
#include "../gui/widgets/toplevelwindow.h"
#include "../gui/widgets/widget.h"
#include "../ui/uimanager.h"

namespace Engine {
namespace UI {
    Handler::Handler(UIManager &ui, const std::string &windowName)
        : mWidget(nullptr)
        , mUI(ui)
        , mWidgetName(windowName)
        , mPointerMoveSlot(this)
        , mPointerDownSlot(this)
        , mPointerUpSlot(this)
    {
    }

    Scene::SceneComponentBase &Handler::getSceneComponent(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return mUI.getSceneComponent(i, init);
    }

    Scene::SceneManager &Handler::sceneMgr(bool init)
    {
        if (init) {
            checkInitState();
        }
        return mUI.sceneMgr(init);
    }

    App::GlobalAPIBase &Handler::getGlobalAPIComponent(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return mUI.getGlobalAPIComponent(i, init);
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

    const MadgineObject *Handler::parent() const
    {
        return &mUI;
    }

    UIManager &Handler::ui(bool init)
    {
        if (init) {
            checkInitState();
        }
        return mUI.getSelf(init);
    }

    bool Handler::init()
    {
        GUI::WidgetBase *widget = mUI.window(false).getWidget(mWidgetName);
        if (!widget) {
            LOG_ERROR(Database::Exceptions::handlerInitializationFailed(mWidgetName));
            return false;
        }
        return init(widget);
    }

    void Handler::finalize()
    {
        mWidget = nullptr;
    }

    bool Handler::init(GUI::WidgetBase *widget)
    {
        assert(widget);
        mWidget = widget;

        mWidget->pointerMoveEvent().connect(mPointerMoveSlot);
        mWidget->pointerDownEvent().connect(mPointerDownSlot);
        mWidget->pointerUpEvent().connect(mPointerUpSlot);

        for (const WindowDescriber &des : mWidgets) {
            GUI::WidgetBase *child = widget->getChildRecursive(des.mWidgetName);

            if (!child) {
                LOG_ERROR(Database::Exceptions::windowNotFound(des.mWidgetName));
                return false;
            }

            if (!des.mInit(child))
                return false;
        }

        mWidgets.clear();

        return true;
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

    void Handler::registerWidget(const std::string &name, std::function<bool(GUI::WidgetBase *)> init)
    {
        assert(!mWidget);
        mWidgets.emplace_back(name, init);
    }

    void Handler::onMouseVisibilityChanged(bool b)
    {
    }

    GUI::WidgetBase *Handler::widget() const
    {
        return mWidget;
    }

}
}
