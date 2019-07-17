#include "../../clientlib.h"

#include "widget.h"

#include "toplevelwindow.h"

#include "../widgetnames.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"

#include "../guisystem.h"
#include "Madgine/app/application.h"

#include "Modules/math/vector4.h"

#include "../vertex.h"


#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"


namespace Engine {

namespace GUI {

    Widget::Widget(const std::string &name, Widget *parent)
        : mName(name)
        , mParent(parent)
        , mWindow(parent->window())
    {
        mWindow.registerWidget(this);
    }

    Widget::Widget(const std::string &name, TopLevelWindow &window)
        : mName(name)
        , mParent(nullptr)
        , mWindow(window)
    {
        mWindow.registerWidget(this);
    }

    Widget::~Widget()
    {
        mWindow.unregisterWidget(this);
    }

    void Widget::setSize(const Matrix3 &size)
    {
        mSize = size;
        if (mParent)
            updateGeometry(window().getScreenSize(), mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
        else
            updateGeometry(window().getScreenSize(), Matrix3::IDENTITY);
    }

    const Matrix3 &Widget::getSize()
    {
        return mSize;
    }

    void Widget::setPos(const Matrix3 &pos)
    {
        mPos = pos;
        if (mParent)
            updateGeometry(window().getScreenSize(), mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
        else
            updateGeometry(window().getScreenSize(), Matrix3::IDENTITY);
    }

    const Matrix3 &Widget::getPos() const
    {
        return mPos;
    }

    Matrix3 Widget::getAbsoluteSize() const
    {
        return mAbsoluteSize;
    }

    Matrix3 Widget::getAbsolutePosition() const
    {
        return mAbsolutePos;
    }

    void Widget::updateGeometry(const Vector3 &screenSize, const Matrix3 &parentSize, const Matrix3 &parentPos)
    {
        mAbsoluteSize = mSize * parentSize;
        mAbsolutePos = mPos * parentSize + parentPos;

        sizeChanged(mAbsoluteSize * screenSize);

        for (Widget *child : uniquePtrToPtr(mChildren)) {
            child->updateGeometry(screenSize, getAbsoluteSize(), getAbsolutePosition());
        }
    }

    void Widget::screenSizeChanged(const Vector3 &screenSize)
    {
        sizeChanged(mAbsoluteSize * screenSize);

        for (Widget *child : uniquePtrToPtr(mChildren)) {
            child->screenSizeChanged(screenSize);
        }
    }

    Class Widget::getClass()
    {
        return Class::WIDGET_CLASS;
    }

    void Widget::destroy()
    {
        if (mParent)
            mParent->destroyChild(this);
        else
            mWindow.destroyTopLevel(this);
    }

    void Widget::releaseInput()
    {
    }

    void Widget::captureInput()
    {
    }

    void Widget::activate()
    {
    }

    const std::string &Widget::getName() const
    {
        return mName;
    }

    const char *Widget::key() const
    {
        return mName.c_str();
    }

    Widget *Widget::createChild(const std::string &name, Class _class)
    {
        switch (_class) {
        case Class::BAR_CLASS:
            return createChildBar(name);
        case Class::CHECKBOX_CLASS:
            return createChildCheckbox(name);
        case Class::LABEL_CLASS:
            return createChildLabel(name);
        case Class::TABWIDGET_CLASS:
            return createChildTabWidget(name);
        case Class::BUTTON_CLASS:
            return createChildButton(name);
        case Class::COMBOBOX_CLASS:
            return createChildCombobox(name);
        case Class::TEXTBOX_CLASS:
            return createChildTextbox(name);
        case Class::SCENEWINDOW_CLASS:
            return createChildSceneWindow(name);
        case Class::IMAGE_CLASS:
            return createChildImage(name);
        default:
            throw 0;
        }
    }

    Widget *Widget::createChildWidget(const std::string &name)
    {
        Widget *w = mChildren.emplace_back(createWidget(name)).get();
        w->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return w;
    }

    Bar *Widget::createChildBar(const std::string &name)
    {
        std::unique_ptr<Bar> p = createBar(name);
        Bar *b = p.get();
        mChildren.emplace_back(std::move(p));
        b->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return b;
    }

    Button *Widget::createChildButton(const std::string &name)
    {
        std::unique_ptr<Button> p = createButton(name);
        Button *b = p.get();
        mChildren.emplace_back(std::move(p));
        b->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return b;
    }

    Checkbox *Widget::createChildCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> p = createCheckbox(name);
        Checkbox *c = p.get();
        mChildren.emplace_back(std::move(p));
        c->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return c;
    }

    Combobox *Widget::createChildCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> p = createCombobox(name);
        Combobox *c = p.get();
        mChildren.emplace_back(std::move(p));
        c->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return c;
    }

    Label *Widget::createChildLabel(const std::string &name)
    {
        std::unique_ptr<Label> p = createLabel(name);
        Label *l = p.get();
        mChildren.emplace_back(std::move(p));
        l->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return l;
    }

    SceneWindow *Widget::createChildSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> p = createSceneWindow(name);
        SceneWindow *s = p.get();
        mChildren.emplace_back(std::move(p));
        s->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return s;
    }

    TabWidget *Widget::createChildTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> p = createTabWidget(name);
        TabWidget *t = p.get();
        mChildren.emplace_back(std::move(p));
        t->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return t;
    }

    Textbox *Widget::createChildTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> p = createTextbox(name);
        Textbox *t = p.get();
        mChildren.emplace_back(std::move(p));
        t->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return t;
    }

    Image *Widget::createChildImage(const std::string &name)
    {
        std::unique_ptr<Image> p = createImage(name);
        Image *i = p.get();
        mChildren.emplace_back(std::move(p));
        i->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return i;
    }

    size_t Widget::depth()
    {
        return mParent ? mParent->depth() + 1 : 0;
    }

    TopLevelWindow &Widget::window()
    {
        return mWindow;
    }

    void Widget::destroyChild(Widget *w)
    {
        auto it = std::find_if(mChildren.begin(), mChildren.end(), [=](const std::unique_ptr<Widget> &ptr) { return ptr.get() == w; });
        assert(it != mChildren.end());
        mChildren.erase(it);
    }

    /*KeyValueMapList Widget::maps()
		{
			return Scope::maps().merge(mChildren, MAP_RO(AbsolutePos, getAbsolutePosition), MAP_RO(AbsoluteSize, getAbsoluteSize), MAP(Visible, isVisible, setVisible), MAP(Size, getSize, setSize), MAP(Position, getPos, setPos));
		}*/

    bool Widget::isVisible() const
    {
        return mVisible;
    }

    void Widget::showModal()
    {
    }

    void Widget::hideModal()
    {
    }

    void Widget::show()
    {
        setVisible(true);
    }

    void Widget::hide()
    {
        setVisible(false);
    }

    void Widget::setVisible(bool b)
    {
        mVisible = b;
    }

    void Widget::setEnabled(bool b)
    {
    }

    Widget *Widget::getChildRecursive(const std::string &name)
    {
        if (name == getName() || name == WidgetNames::thisWidget)
            return this;
        for (const std::unique_ptr<Widget> &w : mChildren)
            if (Widget *f = w->getChildRecursive(name))
                return f;
        return nullptr;
    }

    Widget *Widget::getParent() const
    {
        return mParent;
    }

    bool Widget::injectPointerPress(const Input::PointerEventArgs &arg)
    {
        mPointerDownSignal.emit(arg);
        return true;
    }

    bool Widget::injectPointerRelease(const Input::PointerEventArgs &arg)
    {
        mPointerUpSignal.emit(arg);
        return true;
    }

    bool Widget::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        mPointerMoveSignal.emit(arg);
        return true;
    }

    bool Widget::injectPointerEnter(const Input::PointerEventArgs &arg)
    {
        mPointerEnterSignal.emit(arg);
        return true;
    }

    bool Widget::injectPointerLeave(const Input::PointerEventArgs &arg)
    {
        mPointerLeaveSignal.emit(arg);
        return true;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &Widget::pointerMoveEvent()
    {
        return mPointerMoveSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &Widget::pointerDownEvent()
    {
        return mPointerDownSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &Widget::pointerUpEvent()
    {
        return mPointerUpSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &Widget::pointerEnterEvent()
    {
        return mPointerEnterSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &Widget::pointerLeaveEvent()
    {
        return mPointerLeaveSignal;
    }

    bool Widget::containsPoint(const Vector2 &point, const Vector3 &screenSize, float extend) const
    {
        Vector3 min = mAbsolutePos * screenSize - extend;
        Vector3 max = mAbsoluteSize * screenSize + min + 2 * extend;
        return min.x <= point.x && min.y <= point.y && max.x >= point.x && max.y >= point.y;
    }

    std::vector<Vertex> Widget::vertices(const Vector3 &screenSize)
    {
        return {};
    }

    void *Widget::userData()
    {
        return mUserData;
    }

    void Widget::setUserData(void *userData)
    {
        mUserData = userData;
    }

    std::unique_ptr<Widget> Widget::createWidgetClass(const std::string &name, Class _class)
    {
        switch (_class) {
        case Class::BAR_CLASS:
            return createBar(name);
        case Class::CHECKBOX_CLASS:
            return createCheckbox(name);
        case Class::LABEL_CLASS:
            return createLabel(name);
        case Class::TABWIDGET_CLASS:
            return createTabWidget(name);
        case Class::BUTTON_CLASS:
            return createButton(name);
        case Class::COMBOBOX_CLASS:
            return createCombobox(name);
        case Class::TEXTBOX_CLASS:
            return createTextbox(name);
        case Class::SCENEWINDOW_CLASS:
            return createSceneWindow(name);
        case Class::IMAGE_CLASS:
            return createImage(name);
        default:
            throw 0;
        }
    }
    std::unique_ptr<Widget> Widget::createWidget(const std::string &name)
    {
        return std::make_unique<Widget>(name, this);
    }
    std::unique_ptr<Bar> Widget::createBar(const std::string &name)
    {
        return std::make_unique<Bar>(name, this);
    }
    std::unique_ptr<Button> Widget::createButton(const std::string &name)
    {
        return std::make_unique<Button>(name, this);
    }
    std::unique_ptr<Checkbox> Widget::createCheckbox(const std::string &name)
    {
        return std::make_unique<Checkbox>(name, this);
    }
    std::unique_ptr<Combobox> Widget::createCombobox(const std::string &name)
    {
        return std::make_unique<Combobox>(name, this);
    }
    std::unique_ptr<Image> Widget::createImage(const std::string &name)
    {
        return std::make_unique<Image>(name, this);
    }
    std::unique_ptr<Label> Widget::createLabel(const std::string &name)
    {
        return std::make_unique<Label>(name, this);
    }
    std::unique_ptr<SceneWindow> Widget::createSceneWindow(const std::string &name)
    {
        return std::make_unique<SceneWindow>(name, this);
    }
    std::unique_ptr<TabWidget> Widget::createTabWidget(const std::string &name)
    {
        return std::make_unique<TabWidget>(name, this);
    }
    std::unique_ptr<Textbox> Widget::createTextbox(const std::string &name)
    {
        return std::make_unique<Textbox>(name, this);
    }
    void Widget::sizeChanged(const Vector3 &pixelSize)
    {
    }
}
}

METATABLE_BEGIN(Engine::GUI::Widget)
READONLY_PROPERTY(Widgets, children)
READONLY_PROPERTY(Size, getSize)
METATABLE_END(Engine::GUI::Widget)

RegisterType(Engine::GUI::Widget);