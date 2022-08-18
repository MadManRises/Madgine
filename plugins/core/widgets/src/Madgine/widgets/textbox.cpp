#include "../widgetslib.h"

#include "textbox.h"
#include "widgetmanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Interfaces/input/inputevents.h"

#define STB_TEXTEDIT_KEYTYPE uint16_t

#define STB_TEXTEDIT_STRING Engine::Widgets::Textbox
#define STB_TEXTEDIT_STRINGLEN(obj) obj->mText.size()
#define STB_TEXTEDIT_LAYOUTROW(result, obj, n) obj->layoutRow(result, n)
#define STB_TEXTEDIT_GETWIDTH(obj, n, i) obj->calculateWidth(i, n)
#define STB_TEXTEDIT_KEYTOTEXT(k) static_cast<char>(k)
#define STB_TEXTEDIT_GETCHAR(obj, i) obj->mText.at(i)
#define STB_TEXTEDIT_NEWLINE '\n'
#define STB_TEXTEDIT_DELETECHARS(obj, i, n) obj->mText.erase(i, n)
#define STB_TEXTEDIT_INSERTCHARS(obj, i, c, n) (obj->mText.insert(i, c, n), true)

#define STB_TEXTEDIT_K(key) (static_cast<uint16_t>(key) << 8)

#define STB_TEXTEDIT_K_SHIFT STB_TEXTEDIT_K(Engine::Input::Key::Shift)
#define STB_TEXTEDIT_K_LEFT STB_TEXTEDIT_K(Engine::Input::Key::LeftArrow)
#define STB_TEXTEDIT_K_RIGHT STB_TEXTEDIT_K(Engine::Input::Key::RightArrow)
#define STB_TEXTEDIT_K_UP STB_TEXTEDIT_K(Engine::Input::Key::UpArrow)
#define STB_TEXTEDIT_K_DOWN STB_TEXTEDIT_K(Engine::Input::Key::DownArrow)
#define STB_TEXTEDIT_K_PGUP STB_TEXTEDIT_K(Engine::Input::Key::PageUp)
#define STB_TEXTEDIT_K_PGDOWN STB_TEXTEDIT_K(Engine::Input::Key::PageDown)
#define STB_TEXTEDIT_K_LINESTART STB_TEXTEDIT_K(Engine::Input::Key::Home)
#define STB_TEXTEDIT_K_LINEEND STB_TEXTEDIT_K(Engine::Input::Key::End)
#define STB_TEXTEDIT_K_TEXTSTART 0x4000
#define STB_TEXTEDIT_K_TEXTEND 0x8000000
#define STB_TEXTEDIT_K_DELETE STB_TEXTEDIT_K(Engine::Input::Key::Delete)
#define STB_TEXTEDIT_K_BACKSPACE STB_TEXTEDIT_K(Engine::Input::Key::Backspace) | 8
#define STB_TEXTEDIT_K_UNDO 0x1000000
#define STB_TEXTEDIT_K_REDO 0x2000000

#define STB_TEXTEDIT_IMPLEMENTATION
#include "stb_textedit.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Textbox, Engine::Widgets::WidgetBase)
MEMBER(mText)
NAMED_MEMBER(TextData, mTextRenderData)
NAMED_MEMBER(Image, mImageRenderData)
METATABLE_END(Engine::Widgets::Textbox)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Textbox, Engine::Widgets::WidgetBase)
FIELD(mText)
FIELD(mTextRenderData)
FIELD(mImageRenderData)
SERIALIZETABLE_END(Engine::Widgets::Textbox)

namespace Engine {
namespace Widgets {

    Textbox::Textbox(WidgetManager &manager, WidgetBase *parent)
        : Widget(manager, parent)
    {
        stb_textedit_initialize_state(&mState, true);
    }

    void Textbox::setEditable(bool b)
    {
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> Textbox::vertices(const Vector3 &screenSize)
    {
        const Atlas2::Entry *entry = manager().lookUpImage(mImageRenderData.image());
        if (!entry)
            return {};

        Vector3 pos = (getEffectivePosition() * screenSize) / screenSize;
        Vector3 size = (getEffectiveSize() * screenSize) / screenSize;
        pos.z = depth();

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> returnSet;

        returnSet.push_back({ mImageRenderData.renderImage(pos, size.xy(), screenSize.xy(), *entry), { 0 } });

        if (mTextRenderData.available()) {
            returnSet.push_back(mTextRenderData.render(mText, pos, size, screenSize.xy(), isFocused() && mTextRenderData.animationInterval(1200ms, 600ms) ? mState.cursor : -1));
            if (mState.select_start != mState.select_end) {
                const Atlas2::Entry *blankEntry = manager().lookUpImage("blank_white");
                if (blankEntry) {
                    Vector4 color = { 0.0f, 0.0f, 0.8f, 0.8f };
                    returnSet.push_back({ mTextRenderData.renderSelection(mText, pos, size, screenSize.xy(), *blankEntry, mState.select_start, mState.select_end, color), { 0 } });
                }
            }
        }

        return returnSet;
    }

    WidgetClass Textbox::getClass() const
    {
        return WidgetClass::TEXTBOX;
    }

    bool Textbox::injectPointerClick(const Input::PointerEventArgs &arg)
    {
        stb_textedit_click(this, &mState, arg.screenPosition.x, arg.screenPosition.y);
        return WidgetBase::injectPointerClick(arg);
    }

    bool Textbox::injectDragBegin(const Input::PointerEventArgs &arg)
    {
        stb_textedit_click(this, &mState, arg.screenPosition.x, arg.screenPosition.y);
        return WidgetBase::injectDragBegin(arg);
    }

    bool Textbox::injectDragMove(const Input::PointerEventArgs &arg)
    {
        stb_textedit_drag(this, &mState, arg.screenPosition.x, arg.screenPosition.y);
        return WidgetBase::injectDragMove(arg);
    }

    bool Textbox::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        if (std::isalnum(arg.text)
            || arg.scancode == Input::Key::Control
            || arg.scancode == Input::Key::LeftArrow
            || arg.scancode == Input::Key::RightArrow
            || arg.scancode == Input::Key::UpArrow
            || arg.scancode == Input::Key::DownArrow
            || arg.scancode == Input::Key::Backspace
            || arg.scancode == Input::Key::Delete
            || arg.scancode == Input::Key::Space) {
            uint32_t val = (static_cast<uint16_t>(arg.scancode) << 8) | arg.text;
            stb_textedit_key(this, &mState, val);
        }
        return WidgetBase::injectKeyPress(arg);
    }

    void Textbox::layoutRow(StbTexteditRow *row, size_t i)
    {
        if (!mTextRenderData.available())
            return;

        assert(i == 0);
        Vector3 pos = getAbsolutePosition();
        Vector3 size = getAbsoluteSize();
        Rect2 bb = mTextRenderData.calculateBoundingBox(mText, pos, size);
        row->baseline_y_delta = bb.mSize.y;
        row->num_chars = mText.size();
        row->x0 = bb.mTopLeft.x;
        row->x1 = bb.mTopLeft.x + bb.mSize.x;
        row->ymax = 39; //TODO
        row->ymin = -19;
    }

    float Textbox::calculateWidth(size_t i, size_t n)
    {
        assert(n == 0);

        return mTextRenderData.calculateWidth(StringUtil::substr(mText, i, i + 1), getAbsoluteSize().z);
    }
}
}