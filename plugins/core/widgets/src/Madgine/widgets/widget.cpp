#include "../widgetslib.h"

#include "widget.h"

#include "widgetmanager.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"

#include "vertex.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/fontloader/font.h"

METATABLE_BEGIN(Engine::Widgets::WidgetBase)
READONLY_PROPERTY(Children, children)
READONLY_PROPERTY(Pos, getPos)
READONLY_PROPERTY(Size, getSize)
MEMBER(mVisible)
METATABLE_END(Engine::Widgets::WidgetBase)

SERIALIZETABLE_BEGIN(Engine::Widgets::WidgetBase)
FIELD(mChildren, Serialize::ParentCreator<&Engine::Widgets::WidgetBase::readWidget, &Engine::Widgets::WidgetBase::writeWidget>)
FIELD(mPos)
FIELD(mSize)
SERIALIZETABLE_END(Engine::Widgets::WidgetBase)

namespace Engine {

namespace Widgets {

    WidgetBase::WidgetBase(const std::string &name, WidgetManager &manager, WidgetBase *parent)
        : mName(name)
        , mParent(parent)
        , mManager(manager)
    {
        mManager.registerWidget(this);
    }

    WidgetBase::~WidgetBase()
    {
        mManager.unregisterWidget(this);
    }

    void WidgetBase::setSize(const Matrix3 &size)
    {
        mSize = size;
        if (mParent)
            updateGeometry(manager().getScreenSpace(), mParent->getEffectiveSize(), mParent->getEffectivePosition());
        else
            updateGeometry(manager().getScreenSpace());
    }

    const Matrix3 &WidgetBase::getSize()
    {
        return mSize;
    }

    void WidgetBase::setPos(const Matrix3 &pos)
    {
        mPos = pos;
        if (mParent)
            updateGeometry(manager().getScreenSpace(), mParent->getEffectiveSize(), mParent->getEffectivePosition());
        else
            updateGeometry(manager().getScreenSpace());
    }

    const Matrix3 &WidgetBase::getPos() const
    {
        return mPos;
    }

    Matrix3 WidgetBase::getEffectiveSize() const
    {
        return mEffectiveSize;
    }

    Matrix3 WidgetBase::getEffectivePosition() const
    {
        return mEffectivePos;
    }

    Vector3 WidgetBase::getAbsoluteSize() const
    {
        const Rect2i &screenSpace = mManager.getClientSpace();
        return mEffectiveSize * Vector3 { Vector2 { screenSpace.mSize }, 1.0f };
    }

    Vector3 WidgetBase::getAbsolutePosition() const
    {
        const Rect2i &screenSpace = mManager.getClientSpace();
        return mEffectivePos * Vector3 { Vector2 { screenSpace.mSize }, 1.0f } + Vector3 { Vector2 { screenSpace.mTopLeft }, 0.0f };
    }

    void WidgetBase::updateGeometry(const Rect2i &screenSpace, const Matrix3 &parentSize, const Matrix3 &parentPos)
    {
        mEffectiveSize = mSize * parentSize;
        mEffectivePos = mPos * parentSize + parentPos;

        sizeChanged((mEffectiveSize * Vector3 { Vector2 { screenSpace.mSize }, 1.0f }).floor());

        for (WidgetBase *child : uniquePtrToPtr(mChildren)) {
            child->updateGeometry(screenSpace, mEffectiveSize, mEffectivePos);
        }
    }

    WidgetClass WidgetBase::getClass() const
    {
        return WidgetClass::WIDGET;
    }

    void WidgetBase::destroy()
    {
        if (mParent)
            mParent->destroyChild(this);
        else
            mManager.destroyTopLevel(this);
    }

    const std::string &WidgetBase::getName() const
    {
        return mName;
    }

    void WidgetBase::setName(const std::string &name)
    {
        mManager.updateWidget(this, name);
        mName = name;
    }

    const std::string &WidgetBase::key() const
    {
        return mName;
    }

    WidgetBase *WidgetBase::createChild(const std::string &name, WidgetClass _class)
    {
        switch (_class) {
        case WidgetClass::WIDGET:
            return createChild<WidgetBase>(name);
        case WidgetClass::BAR:
            return createChild<Bar>(name);
        case WidgetClass::CHECKBOX:
            return createChild<Checkbox>(name);
        case WidgetClass::LABEL:
            return createChild<Label>(name);
        case WidgetClass::TABWIDGET:
            return createChild<TabWidget>(name);
        case WidgetClass::BUTTON:
            return createChild<Button>(name);
        case WidgetClass::COMBOBOX:
            return createChild<Combobox>(name);
        case WidgetClass::TEXTBOX:
            return createChild<Textbox>(name);
        case WidgetClass::SCENEWINDOW:
            return createChild<SceneWindow>(name);
        case WidgetClass::IMAGE:
            return createChild<Image>(name);
        default:
            std::terminate();
        }
    }    

    template <typename WidgetType>
    WidgetType *WidgetBase::createChild(const std::string &name)
    {
        std::unique_ptr<WidgetType> p = mManager.create<WidgetType>(name, this);
        WidgetType *w = p.get();
        mChildren.emplace_back(std::move(p));
        w->updateGeometry(mManager.getScreenSpace(), mEffectiveSize, mEffectivePos);
        return w;
    }

    template WidgetBase *WidgetBase::createChild<WidgetBase>(const std::string &);
    template Button *WidgetBase::createChild<Button>(const std::string &);
    template Bar *WidgetBase::createChild<Bar>(const std::string &);
    template Checkbox *WidgetBase::createChild<Checkbox>(const std::string &);
    template Label *WidgetBase::createChild<Label>(const std::string &);
    template TabWidget *WidgetBase::createChild<TabWidget>(const std::string &);
    template Combobox *WidgetBase::createChild<Combobox>(const std::string &);
    template Textbox *WidgetBase::createChild<Textbox>(const std::string &);
    template SceneWindow *WidgetBase::createChild<SceneWindow>(const std::string &);
    template Image *WidgetBase::createChild<Image>(const std::string &);

    size_t WidgetBase::depth()
    {
        return mParent ? mParent->depth() + 1 : 0;
    }

    WidgetManager &WidgetBase::manager()
    {
        return mManager;
    }

    void WidgetBase::destroyChild(WidgetBase *w)
    {
        auto it = std::ranges::find(mChildren, w, projectionToRawPtr);
        assert(it != mChildren.end());
        mChildren.erase(it);
    }

    void WidgetBase::show()
    {
        mVisible = true;
    }

    void WidgetBase::hide()
    {
        mVisible = false;
    }

    void WidgetBase::setEnabled(bool b)
    {
    }

    WidgetBase *WidgetBase::getChildRecursive(const std::string &name)
    {
        if (name == getName() || name.empty())
            return this;
        for (const std::unique_ptr<WidgetBase> &w : mChildren)
            if (WidgetBase *f = w->getChildRecursive(name))
                return f;
        return nullptr;
    }

    void WidgetBase::setParent(WidgetBase *parent)
    {
        auto it = std::ranges::find(mParent->mChildren, this, projectionToRawPtr);
        parent->mChildren.emplace_back(std::move(*it));
        mParent->mChildren.erase(it);
        mParent = parent;
        updateGeometry(mManager.getScreenSpace(), parent->mEffectiveSize, parent->mEffectivePos);
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

    bool WidgetBase::injectAxisEvent(const Input::AxisEventArgs &arg)
    {
        mAxisEventSignal.emit(arg);
        return true;
    }

    bool WidgetBase::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        mKeyPressSignal.emit(arg);
        return true;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerMoveEvent()
    {
        return mPointerMoveSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerDownEvent()
    {
        return mPointerDownSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerUpEvent()
    {
        return mPointerUpSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerEnterEvent()
    {
        return mPointerEnterSignal;
    }

    Threading::SignalStub<const Input::PointerEventArgs &> &WidgetBase::pointerLeaveEvent()
    {
        return mPointerLeaveSignal;
    }

    Threading::SignalStub<const Input::AxisEventArgs &> &WidgetBase::axisEvent()
    {
        return mAxisEventSignal;
    }

    Threading::SignalStub<const Input::KeyEventArgs &> &WidgetBase::keyEvent()
    {
        return mKeyPressSignal;
    }

    bool WidgetBase::containsPoint(const Vector2 &point, const Rect2i &screenSpace, float extend) const
    {
        Vector3 min = mEffectivePos * Vector3 { Vector2 { screenSpace.mSize }, 1.0f } + Vector3 { Vector2 { screenSpace.mTopLeft }, 0.0f } - extend;
        Vector3 max = mEffectiveSize * Vector3 { Vector2 { screenSpace.mSize }, 1.0f } + min + 2 * extend;
        return min.x <= point.x && min.y <= point.y && max.x >= point.x && max.y >= point.y;
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> WidgetBase::vertices(const Vector3 &screenSize)
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

    std::unique_ptr<WidgetBase> WidgetBase::createWidgetClass(const std::string &name, WidgetClass _class)
    {
        return mManager.createWidgetClass(name, _class, this);
    }
    Serialize::StreamResult WidgetBase::readWidget(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget)
    {
        STREAM_PROPAGATE_ERROR(in.beginExtendedRead("Widget", 2));
        std::string name;
        WidgetClass _class;
        STREAM_PROPAGATE_ERROR(read(in, name, "name"));
        STREAM_PROPAGATE_ERROR(read(in, _class, "type"));

        widget = createWidgetClass(name, _class);
        return {};
    }
    const char *WidgetBase::writeWidget(Serialize::FormattedSerializeStream &out, const std::unique_ptr<WidgetBase> &widget) const
    {
        return mManager.writeWidget(out, widget);
    }
    void WidgetBase::sizeChanged(const Vector3i &pixelSize)
    {
    }

    Resources::Resource<Resources::ImageLoader> *WidgetBase::resource() const
    {
        return nullptr;
    }

    std::pair<std::vector<Vertex>, TextureSettings> WidgetBase::renderText(const std::string &text, Vector3 pos, Vector2 size, const Render::Font *font, float fontSize, Vector2 pivot, const Vector3 &screenSize)
    {
        std::vector<Vertex> result;

        size_t textLen = text.size();

        if (textLen == 0)
            return {};

        float scaleX = fontSize / 5.0f / screenSize.x;
        float scaleY = fontSize / 5.0f / screenSize.y;        

        float fullWidth = 0.0f;
        float minY = 0.0f;
        float maxY = 0.0f;

        for (size_t i = 0; i < textLen; ++i) {
            const Render::Glyph &g = font->mGlyphs[text[i]];

            fullWidth += g.mAdvance / 64.0f * scaleX;
            maxY = max(maxY, g.mBearing.y * scaleY);
            minY = min(minY, (g.mBearing.y - g.mSize.y) * scaleY);
        }

        float fullHeight = maxY - minY;

        float cursorX = (size.x - fullWidth) * pivot.x;
        float originY = (size.y - fullHeight) * pivot.y + maxY;

        for (size_t i = 0; i < textLen; ++i) {
            const Render::Glyph &g = font->mGlyphs[text[i]];

            float width = g.mSize.x * scaleX;
            float height = g.mSize.y * scaleY;

            float vPosX1 = cursorX + g.mBearing.x * scaleX;
            float vPosX2 = vPosX1 + width;
            float vPosY1 = originY - g.mBearing.y * scaleY;
            float vPosY2 = vPosY1 + height;

            Vector3 v11 = { vPosX1, vPosY1, pos.z + 0.5f }, v12 = { vPosX2, vPosY1, pos.z + 0.5f }, v21 = { vPosX1, vPosY2, pos.z + 0.5f }, v22 = { vPosX2, vPosY2, pos.z + 0.5f };

            int uvWidth = g.mSize.x;
            int uvHeight = g.mSize.y;

            if (g.mFlipped)
                std::swap(uvWidth, uvHeight);

            Vector2 uvTopLeft = { float(g.mUV.x) / font->mTextureSize.x, float(g.mUV.y) / font->mTextureSize.y };
            Vector2 uvBottomRight = { float(g.mUV.x + uvWidth) / font->mTextureSize.x,
                float(g.mUV.y + uvHeight) / font->mTextureSize.y };

            Vector2 uvTopRight = { uvBottomRight.x, uvTopLeft.y };
            Vector2 uvBottomLeft = { uvTopLeft.x, uvBottomRight.y };

            if (g.mFlipped)
                std::swap(uvTopRight, uvBottomLeft);

            result.push_back({ v11 + pos, { 1, 1, 1, 1 }, uvTopLeft });
            result.push_back({ v12 + pos, { 1, 1, 1, 1 }, uvTopRight });
            result.push_back({ v21 + pos, { 1, 1, 1, 1 }, uvBottomLeft });
            result.push_back({ v21 + pos, { 1, 1, 1, 1 }, uvBottomLeft });
            result.push_back({ v12 + pos, { 1, 1, 1, 1 }, uvTopRight });
            result.push_back({ v22 + pos, { 1, 1, 1, 1 }, uvBottomRight });

            cursorX += g.mAdvance / 64.0f * scaleX;
        }
        return { result, { { font->mTexture->mTextureHandle, Render::TextureType_2D }, TextureFlag_IsDistanceField } };
    }

    void WidgetBase::preRender()
    {
    }

}
}
