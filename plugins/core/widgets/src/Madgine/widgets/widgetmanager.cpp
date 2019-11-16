#include "../widgetslib.h"

#include "widgetmanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "meshloader.h"
#include "programloader.h"

#include "widget.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"

#include "imagedata.h"

#include "vertex.h"

#include "Madgine/gui/toplevelwindow.h"

#include "Interfaces/window/windowapi.h"

UNIQUECOMPONENT(Engine::Widgets::WidgetManager)

METATABLE_BEGIN(Engine::Widgets::WidgetManager)
READONLY_PROPERTY(Widgets, widgets)
MEMBER(mStartupWidget)
METATABLE_END(Engine::Widgets::WidgetManager)

SERIALIZETABLE_BEGIN(Engine::Widgets::WidgetManager)
FIELD(mStartupWidget)
FIELD(mTopLevelWidgets, Serialize::ParentCreator<&Engine::Widgets::WidgetManager::createWidgetClassTuple>)
SERIALIZETABLE_END(Engine::Widgets::WidgetManager)

RegisterType(Engine::Widgets::WidgetManager);

namespace Engine {
namespace Widgets {

    WidgetManager::WidgetManager(GUI::TopLevelWindow &window)
        : SerializableUnit(window, 20)
        , mUIAtlas({ 512, 512 })
    {
    }

    WidgetManager::~WidgetManager()
    {
        assert(mWidgets.empty());
    }

    const char *WidgetManager::key() const
    {
        return "WidgetManager";
    }

    bool WidgetManager::init()
    {
        mProgram = Render::ProgramLoader::getSingleton().getOrCreateManual("ui", [](Render::ProgramLoader::ResourceType *res) {
            return Render::ProgramLoader::getSingleton().create("ui");
        });

        Render::ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "texture", 0);

        mMesh = Resources::MeshLoader::getSingleton().getOrCreateManual("widgetMesh", [](Resources::MeshLoader::ResourceType *res) {
            return Resources::MeshLoader::getSingleton().generate<GUI::Vertex>(3, nullptr, 0);
        });

        mUIAtlasTexture = Render::TextureLoader::getSingleton().getOrCreateManual("widgetUIAtlas", [](Render::TextureLoader::ResourceType *res) { return Render::TextureLoader::getSingleton().create(Render::FORMAT_UNSIGNED_BYTE); });

        mWindow.getRenderWindow()->addRenderPass(this);

        return true;
    }

    void WidgetManager::finalize()
    {
        mWindow.getRenderWindow()->removeRenderPass(this);

        mTopLevelWidgets.clear();

        /*mDefaultTexture.reset();
        mUIAtlasTexture.reset();*/
    }

    WidgetBase *WidgetManager::createTopLevelWidget(const std::string &name)
    {
        std::unique_ptr<WidgetBase> w = createWidget(name);
        return mTopLevelWidgets.emplace_back(std::move(w)).get();
    }

    Bar *WidgetManager::createTopLevelBar(const std::string &name)
    {
        std::unique_ptr<Bar> p = createBar(name);
        Bar *b = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return b;
    }

    Button *WidgetManager::createTopLevelButton(const std::string &name)
    {
        std::unique_ptr<Button> p = createButton(name);
        Button *b = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return b;
    }

    Checkbox *WidgetManager::createTopLevelCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> p = createCheckbox(name);
        Checkbox *c = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return c;
    }

    Combobox *WidgetManager::createTopLevelCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> p = createCombobox(name);
        Combobox *c = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return c;
    }

    Image *WidgetManager::createTopLevelImage(const std::string &name)
    {
        std::unique_ptr<Image> p = createImage(name);
        Image *i = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return i;
    }

    Label *WidgetManager::createTopLevelLabel(const std::string &name)
    {
        std::unique_ptr<Label> p = createLabel(name);
        Label *l = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return l;
    }

    SceneWindow *WidgetManager::createTopLevelSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> p = createSceneWindow(name);
        SceneWindow *s = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return s;
    }

    TabWidget *WidgetManager::createTopLevelTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> p = createTabWidget(name);
        TabWidget *t = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return t;
    }

    Textbox *WidgetManager::createTopLevelTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> p = createTextbox(name);
        Textbox *t = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return t;
    }

    std::unique_ptr<WidgetBase> WidgetManager::createWidgetClass(const std::string &name, WidgetClass _class)
    {
        switch (_class) {
        case WidgetClass::WIDGET_CLASS:
            return createWidget(name);
        case WidgetClass::BAR_CLASS:
            return createBar(name);
        case WidgetClass::CHECKBOX_CLASS:
            return createCheckbox(name);
        case WidgetClass::LABEL_CLASS:
            return createLabel(name);
        case WidgetClass::TABWIDGET_CLASS:
            return createTabWidget(name);
        case WidgetClass::BUTTON_CLASS:
            return createButton(name);
        case WidgetClass::COMBOBOX_CLASS:
            return createCombobox(name);
        case WidgetClass::TEXTBOX_CLASS:
            return createTextbox(name);
        case WidgetClass::SCENEWINDOW_CLASS:
            return createSceneWindow(name);
        case WidgetClass::IMAGE_CLASS:
            return createImage(name);
        default:
            std::terminate();
        }
    }

    std::tuple<std::unique_ptr<WidgetBase>> WidgetManager::createWidgetClassTuple(const std::string &name, WidgetClass _class)
    {
        return { createWidgetClass(name, _class) };
    }

    std::unique_ptr<WidgetBase> WidgetManager::createWidget(const std::string &name)
    {
        std::unique_ptr<WidgetBase> w = std::make_unique<WidgetBase>(name, *this);
        w->hide();
        w->updateGeometry(mClientSpace);
        return w;
    }

    std::unique_ptr<Bar> WidgetManager::createBar(const std::string &name)
    {
        std::unique_ptr<Bar> b = std::make_unique<Bar>(name, *this);
        b->hide();
        b->updateGeometry(mClientSpace);
        return b;
    }

    std::unique_ptr<Button> WidgetManager::createButton(const std::string &name)
    {
        std::unique_ptr<Button> b = std::make_unique<Button>(name, *this);
        b->hide();
        b->updateGeometry(mClientSpace);
        return b;
    }

    std::unique_ptr<Checkbox> WidgetManager::createCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> c = std::make_unique<Checkbox>(name, *this);
        c->hide();
        c->updateGeometry(mClientSpace);
        return c;
    }

    std::unique_ptr<Combobox> WidgetManager::createCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> c = std::make_unique<Combobox>(name, *this);
        c->hide();
        c->updateGeometry(mClientSpace);
        return c;
    }

    std::unique_ptr<Image> WidgetManager::createImage(const std::string &name)
    {
        std::unique_ptr<Image> i = std::make_unique<Image>(name, *this);
        i->hide();
        i->updateGeometry(mClientSpace);
        return i;
    }

    std::unique_ptr<Label> WidgetManager::createLabel(const std::string &name)
    {
        std::unique_ptr<Label> l = std::make_unique<Label>(name, *this);
        l->hide();
        l->updateGeometry(mClientSpace);
        return l;
    }

    std::unique_ptr<SceneWindow> WidgetManager::createSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> s = std::make_unique<SceneWindow>(name, *this);
        s->hide();
        s->updateGeometry(mClientSpace);
        return s;
    }

    std::unique_ptr<TabWidget> WidgetManager::createTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> t = std::make_unique<TabWidget>(name, *this);
        t->hide();
        t->updateGeometry(mClientSpace);
        return t;
    }

    std::unique_ptr<Textbox> WidgetManager::createTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> t = std::make_unique<Textbox>(name, *this);
        t->hide();
        t->updateGeometry(mClientSpace);
        return t;
    }

    static bool propagateInput(WidgetBase *w, const Input::PointerEventArgs &arg, const Rect2i &screenSpace, bool (WidgetBase::*f)(const Input::PointerEventArgs &))
    {
        if (!w->mVisible)
            return false;

        if (!w->containsPoint(arg.position, screenSpace))
            return false;

        for (WidgetBase *c : w->children()) {
            if (propagateInput(c, arg, screenSpace, f))
                return true;
        }
        return (w->*f)(arg);
    }

    bool WidgetManager::injectPointerPress(const Input::PointerEventArgs &arg)
    {

        Input::PointerEventArgs modArgs = arg;
        modArgs.position -= getScreenSpace().mTopLeft;

        Rect2i clientSpace = mClientSpace;

        for (WidgetBase *modalWidget : mModalWidgetList) {
            if (propagateInput(modalWidget, modArgs, clientSpace, &WidgetBase::injectPointerPress))
                return true;
        }

        for (WidgetBase *w : uniquePtrToPtr(mTopLevelWidgets)) {
            if (propagateInput(w, modArgs, clientSpace, &WidgetBase::injectPointerPress))
                return true;
        }

        return false;
    }

    bool WidgetManager::injectPointerRelease(const Input::PointerEventArgs &arg)
    {

        Input::PointerEventArgs modArgs = arg;
        modArgs.position -= Vector2 { static_cast<float>(mWindow.window()->renderX()), static_cast<float>(mWindow.window()->renderY()) } + mClientSpace.mTopLeft;

        Rect2i clientSpace = { { 0, 0 }, mClientSpace.mSize };

        for (WidgetBase *modalWidget : mModalWidgetList) {
            if (propagateInput(modalWidget, modArgs, clientSpace, &WidgetBase::injectPointerRelease))
                return true;
        }

        for (WidgetBase *w : uniquePtrToPtr(mTopLevelWidgets)) {
            if (propagateInput(w, modArgs, clientSpace, &WidgetBase::injectPointerRelease))
                return true;
        }

        return false;
    }

    static WidgetBase *getHoveredWidgetUp(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current)
    {
        if (!current) {
            return nullptr;
        } else if (!current->mVisible || !current->containsPoint(pos, screenSpace)) {
            return getHoveredWidgetUp(pos, screenSpace, current->getParent());
        } else {
            return current;
        }
    }

    WidgetBase *WidgetManager::getHoveredWidgetDown(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current)
    {
        static auto &logOnce = LOG_WARNING("Handle modal widgets for hover");

        const auto &widgets = current ? current->children() : uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mTopLevelWidgets));

        for (WidgetBase *w : widgets) {
            if (w->mVisible && w->containsPoint(pos, screenSpace)) {
                return getHoveredWidgetDown(pos, screenSpace, w);
            }
        }
        return current;
    }

    WidgetBase *WidgetManager::getHoveredWidget(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current)
    {
        return getHoveredWidgetDown(pos, screenSpace, getHoveredWidgetUp(pos, screenSpace, current));
    }

    bool WidgetManager::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        Input::PointerEventArgs modArgs = arg;
        modArgs.position -= Vector2 { static_cast<float>(mWindow.window()->renderX()), static_cast<float>(mWindow.window()->renderY()) };

        Rect2i clientSpace = mClientSpace;

        Vector2 mouse = modArgs.position - clientSpace.mTopLeft;

        if (std::find_if(mWidgets.begin(), mWidgets.end(), [&](const std::pair<const std::string, WidgetBase *> &p) { return p.second == mHoveredWidget; }) == mWidgets.end())
            mHoveredWidget = nullptr;

        WidgetBase *hoveredWidget = getHoveredWidget(mouse, clientSpace, mHoveredWidget);

        if (mHoveredWidget != hoveredWidget) {

            if (mHoveredWidget)
                mHoveredWidget->injectPointerLeave(modArgs);

            mHoveredWidget = hoveredWidget;

            if (mHoveredWidget)
                mHoveredWidget->injectPointerEnter(modArgs);
        }

        if (mHoveredWidget)
            return mHoveredWidget->injectPointerMove(modArgs);

        return false;
    }

    WidgetBase *WidgetManager::currentRoot()
    {
        return mCurrentRoot;
    }

    void WidgetManager::calculateWindowGeometries()
    {
        for (WidgetBase *topLevel : uniquePtrToPtr(mTopLevelWidgets)) {
            topLevel->updateGeometry(getScreenSpace());
        }
    }

    void WidgetManager::destroyTopLevel(WidgetBase *w)
    {
        auto it = std::find_if(mTopLevelWidgets.begin(), mTopLevelWidgets.end(), [=](const std::unique_ptr<WidgetBase> &ptr) { return ptr.get() == w; });
        assert(it != mTopLevelWidgets.end());
        mTopLevelWidgets.erase(it);
    }

    void WidgetManager::clear()
    {
        mTopLevelWidgets.clear();
    }

    bool WidgetManager::isHovered(WidgetBase *w)
    {
        WidgetBase *hovered = mHoveredWidget;
        while (hovered) {
            if (hovered == w)
                return true;
            hovered = hovered->getParent();
        }
        return false;
    }

    WidgetBase *WidgetManager::hoveredWidget()
    {
        return mHoveredWidget;
    }

    WidgetBase *WidgetManager::getWidget(const std::string &name)
    {
        auto it = mWidgets.find(name);
        if (it == mWidgets.end())
            return nullptr;
        return it->second;
    }

    void WidgetManager::registerWidget(WidgetBase *w)
    {
        if (!w->getName().empty()) {
            mWidgets.try_emplace(w->getName(), w);
        }
    }

    void WidgetManager::updateWidget(WidgetBase *w, const std::string &newName)
    {
        unregisterWidget(w);
        if (!newName.empty()) {
            mWidgets.try_emplace(newName, w);
        }
    }

    void WidgetManager::unregisterWidget(WidgetBase *w)
    {
        if (!w->getName().empty()) {
            auto it = mWidgets.find(w->getName());
            assert(it != mWidgets.end());
            if (it->second == w) {
                mWidgets.erase(it);
            }
        }
    }

    void WidgetManager::swapCurrentRoot(WidgetBase *newRoot)
    {
        if (mCurrentRoot)
            mCurrentRoot->hide();
        mCurrentRoot = newRoot;
        newRoot->show();
    }

    void WidgetManager::openModalWidget(WidgetBase *widget)
    {
        widget->show();
        mModalWidgetList.emplace(mModalWidgetList.begin(), widget);
    }

    void WidgetManager::openWidget(WidgetBase *widget)
    {
        widget->show();
    }

    void WidgetManager::closeModalWidget(WidgetBase *widget)
    {
        assert(mModalWidgetList.size() > 0 && mModalWidgetList.front() == widget);
        widget->hide();
        mModalWidgetList.erase(mModalWidgetList.begin());
    }

    void WidgetManager::closeWidget(WidgetBase *widget)
    {
        widget->hide();
    }

    void WidgetManager::openStartupWidget()
    {
        if (mStartupWidget)
            swapCurrentRoot(mStartupWidget);
    }

    void WidgetManager::calculateAvailableScreenSpace(Window::Window *w, Vector3i &pos, Vector3i &size)
    {
    }

    void WidgetManager::onResize(size_t width, size_t height)
    {
        for (WidgetBase *topLevel : uniquePtrToPtr(mTopLevelWidgets)) {
            topLevel->screenSpaceChanged(mClientSpace);
        }
    }

    void WidgetManager::render(Render::RenderTarget *target)
    {
        Rect2i screenSpace = mClientSpace;

        target->setRenderSpace(screenSpace);

            Render::ProgramLoader::getSingleton()
                .bind(*mProgram->loadData());

        std::map<Render::TextureDescriptor, std::vector<GUI::Vertex>> vertices;

        std::queue<std::pair<Widgets::WidgetBase *, int>> q;
        for (Widgets::WidgetBase *w : widgets()) {
            if (w->mVisible) {
                q.push(std::make_pair(w, 0));
            }
        }
        while (!q.empty()) {

            Widgets::WidgetBase *w = q.front().first;
            int depth = q.front().second;
            q.pop();

            for (Widgets::WidgetBase *c : w->children()) {
                if (c->mVisible)
                    q.push(std::make_pair(c, depth + 1));
            }

            std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> localVerticesList = w->vertices(Vector3 { screenSpace.mSize, 1.0f });

            Resources::ImageLoader::ResourceType *resource = w->resource();
            if (resource) {
                auto it = mUIAtlasEntries.find(resource);
                if (it == mUIAtlasEntries.end()) {
                    std::shared_ptr<Resources::ImageData> data = resource->loadData();
                    it = mUIAtlasEntries.try_emplace(resource, mUIAtlas.insert({ data->mWidth, data->mHeight }, [this]() { expandUIAtlas(); })).first;
                    Render::TextureLoader::getSingleton().setSubData(*mUIAtlasTexture->loadData(), { it->second.mArea.mTopLeft.x, it->second.mArea.mTopLeft.y }, it->second.mArea.mSize, data->mBuffer);
                }

                for (std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor> &localVertices : localVerticesList) {

                    std::transform(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]), [&](const GUI::Vertex &v) {
                        return GUI::Vertex {
                            v.mPos,
                            v.mColor,
                            { (it->second.mArea.mSize.x / (512.f * mUIAtlasSize)) * v.mUV.x + it->second.mArea.mTopLeft.x / (512.f * mUIAtlasSize),
                                (it->second.mArea.mSize.y / (512.f * mUIAtlasSize)) * v.mUV.y + it->second.mArea.mTopLeft.y / (512.f * mUIAtlasSize) }
                        };
                    });
                }

            } else {
                for (std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor> &localVertices : localVerticesList) {
                    std::move(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]));
                }
            }
        }

        for (const std::pair<const Render::TextureDescriptor, std::vector<GUI::Vertex>> &p : vertices) {
            if (!p.second.empty()) {

                Render::ProgramLoader::getSingleton().setUniform(*mProgram->loadData(), "hasDistanceField", bool(p.first.mFlags & Render::TextureFlag_IsDistanceField));

                Render::TextureLoader::getSingleton().bind(p.first.mTexture ? *p.first.mTexture : *mUIAtlasTexture->loadData());

                Resources::MeshLoader::getSingleton().update(*mMesh->loadData(), 3, p.second.data(), p.second.size());

                target->renderMesh(mMesh->loadData().get());
            }
        }
    }

    void WidgetManager::expandUIAtlas()
    {
        if (mUIAtlasSize == 0) {
            mUIAtlasSize = 4;
            Render::TextureLoader::getSingleton().setData(*mUIAtlasTexture->loadData(), { mUIAtlasSize * 512, mUIAtlasSize * 512 }, nullptr);
            for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * y });
                }
            }
        } else {
            /*for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * (y + mUIAtlasSize) });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * y });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * (y + mUIAtlasSize) });
                }
            }
            mUIAtlasSize *= 2;
            mUIAtlasTexture.resize({ 512 * mUIAtlasSize, 512 * mUIAtlasSize });*/
            throw "TODO";
        }
    }

    Render::Texture &WidgetManager::uiTexture() const
    {
        return *mUIAtlasTexture->loadData();
    }

}
}