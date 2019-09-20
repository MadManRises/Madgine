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


METATABLE_BEGIN(Engine::GUI::WidgetBase)
READONLY_PROPERTY(Widgets, children)
READONLY_PROPERTY(Size, getSize)
METATABLE_END(Engine::GUI::WidgetBase)

RegisterType(Engine::GUI::WidgetBase);

namespace Engine {

namespace GUI {

    WidgetBase::WidgetBase(const std::string &name, WidgetBase *parent)
        : mName(name)
        , mParent(parent)
        , mWindow(parent->window())
    {
        mWindow.registerWidget(this);
    }

    WidgetBase::WidgetBase(const std::string &name, TopLevelWindow &window)
        : mName(name)
        , mParent(nullptr)
        , mWindow(window)
    {
        mWindow.registerWidget(this);
    }

    WidgetBase::~WidgetBase()
    {
        mWindow.unregisterWidget(this);
    }

    void WidgetBase::setSize(const Matrix3 &size)
    {
        mSize = size;
        if (mParent)
            updateGeometry(window().getScreenSize(), mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
        else
            updateGeometry(window().getScreenSize(), Matrix3::IDENTITY);
    }

    const Matrix3 &WidgetBase::getSize()
    {
        return mSize;
    }

    void WidgetBase::setPos(const Matrix3 &pos)
    {
        mPos = pos;
        if (mParent)
            updateGeometry(window().getScreenSize(), mParent->getAbsoluteSize(), mParent->getAbsolutePosition());
        else
            updateGeometry(window().getScreenSize(), Matrix3::IDENTITY);
    }

    const Matrix3 &WidgetBase::getPos() const
    {
        return mPos;
    }

    Matrix3 WidgetBase::getAbsoluteSize() const
    {
        return mAbsoluteSize;
    }

    Matrix3 WidgetBase::getAbsolutePosition() const
    {
        return mAbsolutePos;
    }

    Vector3 WidgetBase::getActualSize() const
    {
        auto [_, screenSize] = mWindow.getAvailableScreenSpace();
        return mAbsoluteSize * screenSize;
    }

    Vector3 WidgetBase::getActualPosition() const
    {
        auto [screenPos, screenSize] = mWindow.getAvailableScreenSpace();
        return mAbsolutePos * screenSize + screenPos;
    }

    void WidgetBase::updateGeometry(const Vector3 &screenSize, const Matrix3 &parentSize, const Matrix3 &parentPos)
    {
        mAbsoluteSize = mSize * parentSize;
        mAbsolutePos = mPos * parentSize + parentPos;

        sizeChanged(mAbsoluteSize * screenSize);

        for (WidgetBase *child : uniquePtrToPtr(mChildren)) {
            child->updateGeometry(screenSize, getAbsoluteSize(), getAbsolutePosition());
        }
    }

    void WidgetBase::screenSizeChanged(const Vector3 &screenSize)
    {
        sizeChanged(mAbsoluteSize * screenSize);

        for (WidgetBase *child : uniquePtrToPtr(mChildren)) {
            child->screenSizeChanged(screenSize);
        }
    }

    Class WidgetBase::getClass()
    {
        return Class::WIDGET_CLASS;
    }

    void WidgetBase::destroy()
    {
        if (mParent)
            mParent->destroyChild(this);
        else
            mWindow.destroyTopLevel(this);
    }

    void WidgetBase::releaseInput()
    {
    }

    void WidgetBase::captureInput()
    {
    }

    void WidgetBase::activate()
    {
    }

    const std::string &WidgetBase::getName() const
    {
        return mName;
    }

    const char *WidgetBase::key() const
    {
        return mName.c_str();
    }

    WidgetBase *WidgetBase::createChild(const std::string &name, Class _class)
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

    WidgetBase *WidgetBase::createChildWidget(const std::string &name)
    {
        WidgetBase *w = mChildren.emplace_back(createWidget(name)).get();
        w->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return w;
    }

    Bar *WidgetBase::createChildBar(const std::string &name)
    {
        std::unique_ptr<Bar> p = createBar(name);
        Bar *b = p.get();
        mChildren.emplace_back(std::move(p));
        b->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return b;
    }

    Button *WidgetBase::createChildButton(const std::string &name)
    {
        std::unique_ptr<Button> p = createButton(name);
        Button *b = p.get();
        mChildren.emplace_back(std::move(p));
        b->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return b;
    }

    Checkbox *WidgetBase::createChildCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> p = createCheckbox(name);
        Checkbox *c = p.get();
        mChildren.emplace_back(std::move(p));
        c->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return c;
    }

    Combobox *WidgetBase::createChildCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> p = createCombobox(name);
        Combobox *c = p.get();
        mChildren.emplace_back(std::move(p));
        c->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return c;
    }

    Label *WidgetBase::createChildLabel(const std::string &name)
    {
        std::unique_ptr<Label> p = createLabel(name);
        Label *l = p.get();
        mChildren.emplace_back(std::move(p));
        l->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return l;
    }

    SceneWindow *WidgetBase::createChildSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> p = createSceneWindow(name);
        SceneWindow *s = p.get();
        mChildren.emplace_back(std::move(p));
        s->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return s;
    }

    TabWidget *WidgetBase::createChildTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> p = createTabWidget(name);
        TabWidget *t = p.get();
        mChildren.emplace_back(std::move(p));
        t->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return t;
    }

    Textbox *WidgetBase::createChildTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> p = createTextbox(name);
        Textbox *t = p.get();
        mChildren.emplace_back(std::move(p));
        t->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return t;
    }

    Image *WidgetBase::createChildImage(const std::string &name)
    {
        std::unique_ptr<Image> p = createImage(name);
        Image *i = p.get();
        mChildren.emplace_back(std::move(p));
        i->updateGeometry(window().getScreenSize(), getAbsoluteSize());
        return i;
    }

    size_t WidgetBase::depth()
    {
        return mParent ? mParent->depth() + 1 : 0;
    }

    TopLevelWindow &WidgetBase::window()
    {
        return mWindow;
    }

    void WidgetBase::destroyChild(WidgetBase *w)
    {
        auto it = std::find_if(mChildren.begin(), mChildren.end(), [=](const std::unique_ptr<WidgetBase> &ptr) { return ptr.get() == w; });
        assert(it != mChildren.end());
        mChildren.erase(it);
    }

    /*KeyValueMapList Widget::maps()
		{
			return Scope::maps().merge(mChildren, MAP_RO(AbsolutePos, getAbsolutePosition), MAP_RO(AbsoluteSize, getAbsoluteSize), MAP(Visible, isVisible, setVisible), MAP(Size, getSize, setSize), MAP(Position, getPos, setPos));
		}*/

    bool WidgetBase::isVisible() const
    {
        return mVisible;
    }

    void WidgetBase::showModal()
    {
    }

    void WidgetBase::hideModal()
    {
    }

    void WidgetBase::show()
    {
        setVisible(true);
    }

    void WidgetBase::hide()
    {
        setVisible(false);
    }

    void WidgetBase::setVisible(bool b)
    {
        mVisible = b;
    }

    void WidgetBase::setEnabled(bool b)
    {
    }

    WidgetBase *WidgetBase::getChildRecursive(const std::string &name)
    {
        if (name == getName() || name == WidgetNames::thisWidget)
            return this;
        for (const std::unique_ptr<WidgetBase> &w : mChildren)
            if (WidgetBase *f = w->getChildRecursive(name))
                return f;
        return nullptr;
    }

    WidgetBase *WidgetBase::getParent() const
    {
        return mParent;
    }

    bool WidgetBase::injectPointerPress(const Input::PointerEventArgs &arg)
    {
        mPointerDownSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectPointerRelease(const Input::PointerEventArgs &arg)
    {
        mPointerUpSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        mPointerMoveSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectPointerEnter(const Input::PointerEventArgs &arg)
    {
        mPointerEnterSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectPointerLeave(const Input::PointerEventArgs &arg)
    {
        mPointerLeaveSignal.emit(arg);
        return true;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerMoveEvent()
    {
        return mPointerMoveSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerDownEvent()
    {
        return mPointerDownSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerUpEvent()
    {
        return mPointerUpSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerEnterEvent()
    {
        return mPointerEnterSignal;
    }

    SignalSlot::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerLeaveEvent()
    {
        return mPointerLeaveSignal;
    }

    bool WidgetBase::containsPoint(const Vector2 &point, const Vector3 &screenSize, const Vector3 &screenPos, float extend) const
    {
        Vector3 min = mAbsolutePos * screenSize + screenPos - extend;
        Vector3 max = mAbsoluteSize * screenSize + min + 2 * extend;
        return min.x <= point.x && min.y <= point.y && max.x >= point.x && max.y >= point.y;
    }

    std::pair<std::vector<Vertex>, uint32_t> WidgetBase::vertices(const Vector3 &screenSize)
    {
        return {};
    }

    void *WidgetBase::userData()
    {
        return mUserData;
    }

    void WidgetBase::setUserData(void *userData)
    {
        mUserData = userData;
    }

    std::unique_ptr<WidgetBase> WidgetBase::createWidgetClass(const std::string &name, Class _class)
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
    std::unique_ptr<WidgetBase> WidgetBase::createWidget(const std::string &name)
    {
        return std::make_unique<WidgetBase>(name, this);
    }
    std::unique_ptr<Bar> WidgetBase::createBar(const std::string &name)
    {
        return std::make_unique<Bar>(name, this);
    }
    std::unique_ptr<Button> WidgetBase::createButton(const std::string &name)
    {
        return std::make_unique<Button>(name, this);
    }
    std::unique_ptr<Checkbox> WidgetBase::createCheckbox(const std::string &name)
    {
        return std::make_unique<Checkbox>(name, this);
    }
    std::unique_ptr<Combobox> WidgetBase::createCombobox(const std::string &name)
    {
        return std::make_unique<Combobox>(name, this);
    }
    std::unique_ptr<Image> WidgetBase::createImage(const std::string &name)
    {
        return std::make_unique<Image>(name, this);
    }
    std::unique_ptr<Label> WidgetBase::createLabel(const std::string &name)
    {
        return std::make_unique<Label>(name, this);
    }
    std::unique_ptr<SceneWindow> WidgetBase::createSceneWindow(const std::string &name)
    {
        return std::make_unique<SceneWindow>(name, this);
    }
    std::unique_ptr<TabWidget> WidgetBase::createTabWidget(const std::string &name)
    {
        return std::make_unique<TabWidget>(name, this);
    }
    std::unique_ptr<Textbox> WidgetBase::createTextbox(const std::string &name)
    {
        return std::make_unique<Textbox>(name, this);
    }
    void WidgetBase::sizeChanged(const Vector3 &pixelSize)
    {
    }

	const MetaTable *WidgetBase::type()
    {
        return &table<WidgetBase>();
    }

	Resources::ImageLoader::ResourceType *WidgetBase::resource() const
    {
        return nullptr;
    }

}
}
