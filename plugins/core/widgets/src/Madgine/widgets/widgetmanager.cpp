#include "../widgetslib.h"

#include "widgetmanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/meshloader/gpumeshloader.h"
#include "Madgine/pipelineloader/pipelineloader.h"

#include "widget.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "textbox.h"

#include "Madgine/imageloader/imagedata.h"

#include "Madgine/window/mainwindow.h"

#include "Meta/math/atlas2.h"

#include "Interfaces/input/inputevents.h"

#include "Madgine/imageloader/imageloader.h"
#include "Madgine/meshloader/meshloader.h"
#include "Madgine/textureloader/textureloader.h"

#include "Madgine/render/rendertarget.h"

#include "Madgine/render/shadinglanguage/sl.h"

#include "Generic/areaview.h"

#define SL_SHADER ui
#include INCLUDE_SL_SHADER

UNIQUECOMPONENT(Engine::Widgets::WidgetManager)

METATABLE_BEGIN(Engine::Widgets::WidgetManager)
READONLY_PROPERTY(Widgets, widgets)
MEMBER(mStartupWidget)
METATABLE_END(Engine::Widgets::WidgetManager)

SERIALIZETABLE_BEGIN(Engine::Widgets::WidgetManager)
FIELD(mStartupWidget)
FIELD(mTopLevelWidgets, Serialize::ParentCreator<&Engine::Widgets::WidgetManager::readWidget, &Engine::Widgets::WidgetManager::writeWidget>)
SERIALIZETABLE_END(Engine::Widgets::WidgetManager)

namespace Engine {
namespace Widgets {

    static float sDragDistanceThreshold = 2.0f;
    static std::chrono::steady_clock::duration sDragTimeThreshold = 150ms;

    struct WidgetManager::WidgetManagerData {

        Render::PipelineLoader::Instance mPipeline;
        Render::GPUMeshLoader::Ptr mMesh;

        Resources::ImageLoader::Handle mDefaultTexture;
        Render::TextureLoader::Ptr mUIAtlasTexture;
        std::set<Resources::ImageLoader::Handle> mImageLoadingTasks;
        Atlas2 mUIAtlas { { 2048, 2048 } };
        int mUIAtlasSize = 0;
        std::map<Resources::ImageLoader::Resource *, Atlas2::Entry> mUIAtlasEntries;

        void expandUIAtlas()
        {
            if (mUIAtlasSize == 0) {
                mUIAtlasSize = 1;
                mUIAtlasTexture.setData({ mUIAtlasSize * 2048, mUIAtlasSize * 2048 }, {});
                for (int x = 0; x < mUIAtlasSize; ++x) {
                    for (int y = 0; y < mUIAtlasSize; ++y) {
                        mUIAtlas.addBin({ 2048 * x, 2048 * y });
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
    };

    WidgetManager::WidgetManager(Window::MainWindow &window)
        : VirtualData(window, 20)
        , mData(std::make_unique<WidgetManagerData>())
    {
    }

    WidgetManager::~WidgetManager()
    {
        assert(mWidgets.empty());
    }

    std::string_view WidgetManager::key() const
    {
        return "WidgetManager";
    }

    Threading::Task<bool> WidgetManager::init()
    {
        if (!co_await MainWindowComponentBase::init())
            co_return false;

        mData->mPipeline.createStatic({ .vs = "ui", .ps = "ui", .format = type_holder<Vertex>, .bufferSizes = { 0, 0, sizeof(WidgetsPerObject) } });

        mData->mMesh.create({ 3, std::vector<Vertex> {} });

        mData->mUIAtlasTexture.create(Render::TextureType_2D, Render::FORMAT_RGBA8);

        mData->mDefaultTexture.load("default_tex");

        mWindow.getRenderWindow()->addRenderPass(this);

        co_return true;
    }

    Threading::Task<void> WidgetManager::finalize()
    {
        mTopLevelWidgets.clear();

        mData->mDefaultTexture.reset();

        mData->mUIAtlasTexture.reset();

        mData->mMesh.reset();

        mData->mPipeline.reset();

        co_await MainWindowComponentBase::finalize();

        co_return;
    }

    template <typename WidgetType>
    std::unique_ptr<WidgetType> WidgetManager::create(WidgetBase *parent)
    {
        return std::make_unique<WidgetType>(*this, parent);
    }

    template std::unique_ptr<WidgetBase> WidgetManager::create<WidgetBase>(WidgetBase *);
    template std::unique_ptr<Bar> WidgetManager::create<Bar>(WidgetBase *);
    template std::unique_ptr<Checkbox> WidgetManager::create<Checkbox>(WidgetBase *);
    template std::unique_ptr<Label> WidgetManager::create<Label>(WidgetBase *);
    template std::unique_ptr<Button> WidgetManager::create<Button>(WidgetBase *);
    template std::unique_ptr<Combobox> WidgetManager::create<Combobox>(WidgetBase *);
    template std::unique_ptr<Textbox> WidgetManager::create<Textbox>(WidgetBase *);
    template std::unique_ptr<SceneWindow> WidgetManager::create<SceneWindow>(WidgetBase *);
    template std::unique_ptr<Image> WidgetManager::create<Image>(WidgetBase *);

    template <typename WidgetType>
    WidgetType *WidgetManager::createTopLevel()
    {
        std::unique_ptr<WidgetType> p = create<WidgetType>();
        WidgetType *w = p.get();
        w->hide();
        w->updateGeometry(mClientSpace);
        mTopLevelWidgets.emplace_back(std::move(p));
        return w;
    }

    template DLL_EXPORT WidgetBase *WidgetManager::createTopLevel<WidgetBase>();

    std::unique_ptr<WidgetBase> WidgetManager::createWidgetByClass(WidgetClass _class, WidgetBase *parent)
    {
        std::unique_ptr<WidgetBase> w = [=]() -> std::unique_ptr<WidgetBase> {
            switch (_class) {
            case WidgetClass::WIDGET:
                return create<WidgetBase>(parent);
            case WidgetClass::BAR:
                return create<Bar>(parent);
            case WidgetClass::CHECKBOX:
                return create<Checkbox>(parent);
            case WidgetClass::LABEL:
                return create<Label>(parent);
            case WidgetClass::BUTTON:
                return create<Button>(parent);
            case WidgetClass::COMBOBOX:
                return create<Combobox>(parent);
            case WidgetClass::TEXTBOX:
                return create<Textbox>(parent);
            case WidgetClass::SCENEWINDOW:
                return create<SceneWindow>(parent);
            case WidgetClass::IMAGE:
                return create<Image>(parent);
            default:
                std::terminate();
            }
        }();
        if (!parent) {
            w->hide();
            w->updateGeometry(mClientSpace);
        }
        return w;
    }

    Serialize::StreamResult WidgetManager::readWidget(Serialize::FormattedSerializeStream &in, std::unique_ptr<WidgetBase> &widget)
    {
        STREAM_PROPAGATE_ERROR(in.beginExtendedRead("Widget", 1));
        WidgetClass _class;
        STREAM_PROPAGATE_ERROR(read(in, _class, "type"));

        widget = createWidgetByClass(_class);
        return {};
    }

    const char *WidgetManager::writeWidget(Serialize::FormattedSerializeStream &out, const std::unique_ptr<WidgetBase> &widget) const
    {
        out.beginExtendedWrite("Widget", 1);
        write(out, widget->getClass(), "type");

        return "Widget";
    }

    WidgetBase *WidgetManager::propagateInput(WidgetBase *w, const Input::PointerEventArgs &arg)
    {
        if (!w->mVisible)
            return nullptr;

        if (!w->containsPoint(Vector2 { static_cast<float>(arg.windowPosition.x), static_cast<float>(arg.windowPosition.y) }, { { 0, 0 }, mClientSpace.mSize }))
            return nullptr;

        for (WidgetBase *c : w->children()) {
            if (WidgetBase *target = propagateInput(c, arg))
                return target;
        }
        return /*passthrough ? nullptr : */ w;
    }

    bool WidgetManager::injectPointerPress(const Input::PointerEventArgs &arg)
    {
        assert(mDragStartEvent.button != arg.button);
        if (mDragStartEvent.button != Input::MouseButton::NO_BUTTON)
            return true;

        Input::PointerEventArgs widgetArg = arg;
        widgetArg.windowPosition = { widgetArg.windowPosition.x - mClientSpace.mTopLeft.x, widgetArg.windowPosition.y - mClientSpace.mTopLeft.y };

        WidgetBase *target = nullptr;

        for (WidgetBase *modalWidget : mModalWidgetList) {
            target = propagateInput(modalWidget, widgetArg);
            if (target)
                break;
        }

        if (!target) {
            for (WidgetBase *w : uniquePtrToPtr(mTopLevelWidgets)) {
                target = propagateInput(w, widgetArg);
                if (target)
                    break;
            }
        }

        if (target) {
            mFocusedWidget = target;

            mDragStartEvent = arg;

            mDragStartTime = std::chrono::steady_clock::now();

            return true;
        }

        return false;
    }

    bool WidgetManager::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        for (WidgetBase *modalWidget : mModalWidgetList) {
            while (modalWidget) {
                if (modalWidget->injectKeyPress(arg))
                    return true;
                modalWidget = modalWidget->getParent();
            }
        }

        WidgetBase *w = mFocusedWidget;
        while (w) {
            if (w->injectKeyPress(arg))
                return true;
            w = w->getParent();
        }

        return false;
    }

    bool WidgetManager::injectPointerRelease(const Input::PointerEventArgs &arg)
    {
        Input::PointerEventArgs widgetArg = arg;
        widgetArg.windowPosition = { widgetArg.windowPosition.x - mClientSpace.mTopLeft.x, widgetArg.windowPosition.y - mClientSpace.mTopLeft.y };

        if (mDragStartEvent.button != arg.button)
            return false;

        assert(mFocusedWidget);

        if (mDragging) {
            mFocusedWidget->injectDragEnd(widgetArg);
            mDragging = false;
        } else {
            mFocusedWidget->injectPointerClick(widgetArg);
        }

        mDragStartEvent.button = Input::MouseButton::NO_BUTTON;

        return true;
    }

    WidgetBase *WidgetManager::getHoveredWidgetUp(const Vector2 &pos, WidgetBase *current)
    {
        if (!current) {
            return nullptr;
        } else if (!current->mVisible || !current->containsPoint(pos, { { 0, 0 }, mClientSpace.mSize })) {
            return getHoveredWidgetUp(pos, current->getParent());
        } else {
            return current;
        }
    }

    WidgetBase *WidgetManager::getHoveredWidgetDown(const Vector2 &pos, WidgetBase *current)
    {
        LOG_WARNING_ONCE("TODO: Handle modal widgets for hover (WidgetManager)");

        if (current) {
            for (WidgetBase *w : current->children()) {
                if (w->mVisible && w->containsPoint(pos, { { 0, 0 }, mClientSpace.mSize })) {
                    return getHoveredWidgetDown(pos, w);
                }
            }
        } else {
            for (WidgetBase *w : uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mTopLevelWidgets))) {
                if (w->mVisible && w->containsPoint(pos, { { 0, 0 }, mClientSpace.mSize })) {
                    return getHoveredWidgetDown(pos, w);
                }
            }
        }

        return current;
    }

    WidgetBase *WidgetManager::getHoveredWidget(const Vector2 &pos, WidgetBase *current)
    {
        return getHoveredWidgetDown(pos, getHoveredWidgetUp(pos, current));
    }

    bool WidgetManager::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        if (std::ranges::find(mWidgets, mHoveredWidget) == mWidgets.end())
            mHoveredWidget = nullptr;

        Input::PointerEventArgs widgetArg = arg;
        widgetArg.windowPosition = { widgetArg.windowPosition.x - mClientSpace.mTopLeft.x, widgetArg.windowPosition.y - mClientSpace.mTopLeft.y };

        if (mDragStartEvent.button != Input::MouseButton::NO_BUTTON) {

            if (!mDragging) {
                InterfacesVector dist = arg.screenPosition - mDragStartEvent.screenPosition;
                if (std::abs(dist.x) + std::abs(dist.y) > sDragDistanceThreshold && std::chrono::steady_clock::now() - mDragStartTime > sDragTimeThreshold) {

                    mFocusedWidget->injectDragBegin(mDragStartEvent);
                    mDragging = true;
                }
            }

            if (mDragging) {
                mFocusedWidget->injectDragMove(widgetArg);
            }

            return false;
        }

        WidgetBase *hoveredWidget = getHoveredWidget(Vector2 { static_cast<float>(widgetArg.windowPosition.x), static_cast<float>(widgetArg.windowPosition.y) }, mHoveredWidget);

        if (mHoveredWidget != hoveredWidget) {

            if (mHoveredWidget)
                mHoveredWidget->injectPointerLeave(widgetArg);

            mHoveredWidget = hoveredWidget;

            if (mHoveredWidget)
                mHoveredWidget->injectPointerEnter(widgetArg);
        }

        if (mHoveredWidget)
            return mHoveredWidget->injectPointerMove(widgetArg);

        return false;
    }

    bool WidgetManager::injectAxisEvent(const Input::AxisEventArgs &arg)
    {
        if (std::ranges::find(mWidgets, mHoveredWidget) == mWidgets.end())
            mHoveredWidget = nullptr;

        if (mHoveredWidget)
            return mHoveredWidget->injectAxisEvent(arg);

        return false;
    }

    WidgetBase *WidgetManager::currentRoot()
    {
        return mCurrentRoot;
    }

    void WidgetManager::destroyTopLevel(WidgetBase *w)
    {
        auto it = std::ranges::find(mTopLevelWidgets, w, projectionToRawPtr);
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

    WidgetBase *Engine::Widgets::WidgetManager::focusedWidget()
    {
        return mFocusedWidget;
    }

    WidgetBase *WidgetManager::getWidget(std::string_view name)
    {
        auto it = std::ranges::find(mWidgets, name, &WidgetBase::mName);
        if (it == mWidgets.end())
            return nullptr;
        return *it;
    }

    void WidgetManager::registerWidget(WidgetBase *w)
    {
        auto pib = mWidgets.insert(w);
        assert(pib.second);
    }

    void WidgetManager::unregisterWidget(WidgetBase *w)
    {
        auto count = mWidgets.erase(w);
        assert(count == 1);
    }

    void WidgetManager::swapCurrentRoot(WidgetBase *newRoot)
    {
        if (mCurrentRoot)
            mCurrentRoot->hide();
        mHoveredWidget = nullptr;
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

    void WidgetManager::onResize(const Rect2i &space)
    {
        MainWindowComponentBase::onResize(space);
        for (WidgetBase *topLevel : uniquePtrToPtr(mTopLevelWidgets)) {
            topLevel->updateGeometry(space);
        }
    }

    void WidgetManager::render(Render::RenderTarget *target, size_t iteration)
    {
        target->pushAnnotation("WidgetManager");

        MainWindowComponentBase::render(target, iteration);

        std::map<TextureSettings, std::vector<Vertex>> vertices;

        for (Widgets::WidgetBase *w : visibleWidgets()) {

            std::vector<std::pair<std::vector<Vertex>, TextureSettings>> localVerticesList = w->vertices(Vector3 { Vector2 { mClientSpace.mSize }, 1.0f });

            for (auto &[localVertices, tex] : localVerticesList) {
                if (!localVertices.empty())
                    std::ranges::move(localVertices, std::back_inserter(vertices[tex]));
            }
        }

        for (auto &[tex, vertices] : vertices) {
            assert(!vertices.empty());

            {
                auto parameters = mData->mPipeline->mapParameters<WidgetsPerObject>(2);
                parameters->hasDistanceField = bool(tex.mFlags & TextureFlag_IsDistanceField);
            }

            if (tex.mTexture.mTextureHandle)
                mData->mPipeline->bindTextures({ tex.mTexture });
            else
                mData->mPipeline->bindTextures({ { mData->mUIAtlasTexture->mTextureHandle, Render::TextureType_2D } });

            mData->mMesh.update({ 3, std::move(vertices) });

            mData->mPipeline->renderMesh(mData->mMesh);
        }

        target->popAnnotation();
    }

    Generator<WidgetBase *> WidgetManager::visibleWidgets()
    {
        std::queue<Widgets::WidgetBase *> q;
        for (Widgets::WidgetBase *w : widgets()) {
            if (w->mVisible) {
                q.push(w);
            }
        }
        while (!q.empty()) {
            Widgets::WidgetBase *w = q.front();
            q.pop();

            for (Widgets::WidgetBase *c : w->children()) {
                if (c->mVisible)
                    q.push(c);
            }

            co_yield w;
        }
    }

    void WidgetManager::preRender()
    {
        for (WidgetBase *w : visibleWidgets()) {
            w->preRender();
        }
    }

    const Render::Texture &WidgetManager::uiTexture() const
    {
        return *mData->mUIAtlasTexture;
    }

    Threading::SignalStub<> &WidgetManager::updatedSignal()
    {
        return mUpdatedSignal;
    }

    int WidgetManager::priority() const
    {
        return mPriority;
    }

    void WidgetManager::onActivate(bool active)
    {
        if (active) {
            openStartupWidget();
            mUpdatedSignal.emit();
        }
    }

    const Atlas2::Entry *WidgetManager::lookUpImage(Resources::ImageLoader::Resource *image)
    {
        auto it = mData->mUIAtlasEntries.find(image);
        if (it == mData->mUIAtlasEntries.end()) {
            Resources::ImageLoader::Handle data = image ? image->loadData() : mData->mDefaultTexture;
            if (!data.available()) {
                mData->mImageLoadingTasks.emplace(std::move(data));
                return nullptr;
            } else {

                size_t width = data->mSize.x;
                size_t height = data->mSize.y;

                assert(data->mChannels == 4);

                const uint32_t *source = static_cast<const uint32_t *>(data->mBuffer.mData);

                std::vector<uint32_t> targetBuffer;
                targetBuffer.resize((width + 2) * (height + 2));

                AreaView<uint32_t, 2> targetArea { targetBuffer.data(), { width + 2, height + 2 } };

                AreaView<const uint32_t, 2> sourceArea { source, { width, height } };

                std::ranges::copy(sourceArea, targetArea.subArea({ 1, 1 }, { width, height }).begin());
                std::ranges::copy(sourceArea.subArea({ 0, 0 }, { 1, height }), targetArea.subArea({ 0, 1 }, { 1, height }).begin());
                std::ranges::copy(sourceArea.subArea({ width - 1, 0 }, { 1, height }), targetArea.subArea({ width + 1, 1 }, { 1, height }).begin());
                std::ranges::copy(sourceArea.subArea({ 0, 0 }, { width, 1 }), targetArea.subArea({ 1, 0 }, { width, 1 }).begin());
                std::ranges::copy(sourceArea.subArea({ 0, height - 1 }, { width, 1 }), targetArea.subArea({ 1, height + 1 }, { width, 1 }).begin());

                targetArea[0][0] = sourceArea[0][0];
                targetArea[0][width + 1] = sourceArea[0][width - 1];
                targetArea[height + 1][0] = sourceArea[height - 1][0];
                targetArea[height + 1][width + 1] = sourceArea[height - 1][width - 1];

                Atlas2::Entry entry = mData->mUIAtlas.insert(data->mSize + Vector2i { 2, 2 }, [this]() { mData->expandUIAtlas(); });
                it = mData->mUIAtlasEntries.try_emplace(image, entry).first;
                mData->mUIAtlasTexture.setSubData(entry.mArea.mTopLeft, data->mSize + Vector2i { 2, 2 }, targetBuffer);
                mData->mImageLoadingTasks.erase(data);
            }
        }
        return &it->second;
    }

    const Atlas2::Entry *WidgetManager::lookUpImage(std::string_view name)
    {
        return lookUpImage(Resources::ImageLoader::getSingleton().get(name));
    }

}
}