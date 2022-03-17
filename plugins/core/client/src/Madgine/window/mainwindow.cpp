#include "../clientlib.h"
#include "serialize/filesystem/filesystemlib.h"

#include "mainwindow.h"
#include "mainwindowcomponent.h"
#include "toolwindow.h"

#include "Generic/container/reverseIt.h"

#include "Interfaces/filesystem/api.h"
#include "Interfaces/window/windowapi.h"
#include "Interfaces/window/windowsettings.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/configs/controlled.h"
#include "Meta/serialize/formatter/iniformatter.h"
#include "Meta/serialize/serializetable_impl.h"
#include "serialize/filesystem/filemanager.h"

#include "../render/rendercontext.h"
#include "../render/rendertarget.h"

METATABLE_BEGIN(Engine::Window::MainWindow)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::Window::MainWindow)

SERIALIZETABLE_BEGIN(Engine::Window::MainWindow)
FIELD(mComponents, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::Window::MainWindowComponentBase>>>)
SERIALIZETABLE_END(Engine::Window::MainWindow)

namespace Engine {
namespace Window {

    static std::queue<WindowData> sTestPositions;
    static std::mutex sTestPositionMutex;

    bool MainWindowComponentComparator::operator()(const std::unique_ptr<MainWindowComponentBase> &first, const std::unique_ptr<MainWindowComponentBase> &second) const
    {
        return first->mPriority < second->mPriority;
    }

    int MainWindowComponentComparator::traits::to_cmp_type(const item_type &value)
    {
        return value->mPriority;
    }

    /**
     * @brief Creates a MainWindow and sets up its TaskQueue
     * @param settings settings for the creation of OSWindow
     * 
     * The settings are stored by reference. Instantiates all MainWindowComponents. 
     * Initialization/Deinitialization-tasks of the MadgineObject are registered as
     * setup steps in the TaskQueue. render() is registered as repeated task to the
     * TaskQueue. 
    */
    MainWindow::MainWindow(const WindowSettings &settings)
        : mSettings(settings)
        , mTaskQueue("FrameLoop", true)
        , mComponents(*this)
    {
        mTaskQueue.addSetupSteps(
            [this]() { return callInit(); },
            [this]() { return callFinalize(); });
    }

    /**
     * @brief default destructor 
    */
    MainWindow::~MainWindow() = default;

    /**
     * @brief 
     * @return 
    */
    Threading::Task<bool> MainWindow::init()
    {
        WindowSettings settings = mSettings;

        if (!sTestPositions.empty()) {
            std::unique_lock lock { sTestPositionMutex };
            settings.mData = sTestPositions.front();
            sTestPositions.pop();
        } else if (settings.mRestoreGeometry) {
            Filesystem::FileManager mgr { "MainWindow-Layout" };

            Filesystem::Path path = Filesystem::appDataPath() / "mainwindow.ini";

            if (Serialize::FormattedSerializeStream in = mgr.openRead(path, std::make_unique<Serialize::IniFormatter>())) {
                Serialize::StreamResult result = read(in, settings.mData, nullptr);
                if (result.mState != Serialize::StreamState::OK) {
                    LOG_ERROR("Error loading MainWindow-Layout from " << path << ": \n"
                                                                      << result);
                }
            }
        }

        mOsWindow = sCreateWindow(settings, this);

        assert(!mRenderContext);
        mRenderContext.emplace(&mTaskQueue);
        mRenderWindow = (*mRenderContext)->createRenderWindow(mOsWindow);

        for (const std::unique_ptr<MainWindowComponentBase> &comp : components()) {
            bool result = co_await comp->callInit();
            assert(result);
        }

        applyClientSpaceResize();

        mTaskQueue.addRepeatedTask([this]() {
            render();
        },
            std::chrono::microseconds(/*1*/ 000000 / 60), this);

        co_return true;
    }

    /**
     * @brief 
     * @return 
    */
    Threading::Task<void> MainWindow::finalize()
    {
        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            co_await comp->callFinalize();
        }

        (*mRenderContext)->unloadAllResources();

        co_await mTaskQueue;

        mRenderWindow.reset();
        mRenderContext.reset();

        if (mOsWindow) {
            storeWindowData();
            mOsWindow->destroy();
            mOsWindow = nullptr;
        }

        co_return;
    }

    /**
     * @brief 
    */
    void MainWindow::render()
    {
        (*mRenderContext)->render();
        mOsWindow->update();
        for (ToolWindow &window : mToolWindows)
            window.osWindow()->update();
    }

    /**
     * @brief 
     * @param i 
     * @return 
    */
    MainWindowComponentBase &MainWindow::getWindowComponent(size_t i)
    {
        return mComponents.get(i);
    }

    /**
     * @brief 
     * @param settings 
     * @return 
    */
    ToolWindow *MainWindow::createToolWindow(const WindowSettings &settings)
    {
        return &mToolWindows.emplace_back(*this, settings);
    }

    /**
     * @brief 
     * @param w 
    */
    void MainWindow::destroyToolWindow(ToolWindow *w)
    {
        auto it = std::ranges::find(mToolWindows, w, projectionAddressOf);
        assert(it != mToolWindows.end());
        mToolWindows.erase(it);
    }

    /**
     * @brief Returns a pointer to the OSWindow 
     * @return the OSWindow
    */
    OSWindow *MainWindow::osWindow() const
    {
        return mOsWindow;
    }

    /**
     * @brief Returns a pointer to the RenderContext
     * @return the RenderContext
    */
    Render::RenderContext *MainWindow::getRenderer()
    {
        return *mRenderContext;
    }

    /**
     * @brief Returns the pointer to the RenderWindow
     * @return the RenderWindow
    */
    Render::RenderTarget *MainWindow::getRenderWindow()
    {
        return mRenderWindow.get();
    }

    /**
     * @brief Returns a pointer to the TaskQueue
     * @return the TaskQueue
    */
    Threading::TaskQueue *MainWindow::taskQueue()
    {
        return &mTaskQueue;
    }

    /**
     * @brief 
    */
    void MainWindow::shutdown()
    {
        mTaskQueue.stop();
    }

    /**
     * @brief 
     * @return 
    */
    Rect2i MainWindow::getScreenSpace()
    {
        if (!mOsWindow)
            return { { 0, 0 }, { 0, 0 } };
        InterfacesVector pos = mOsWindow->renderPos(), size = mOsWindow->renderSize();
        return {
            { pos.x, pos.y }, { size.x, size.y }
        };
    }

    /**
     * @brief 
     * @param component 
    */
    void MainWindow::applyClientSpaceResize(MainWindowComponentBase *component)
    {
        if (!mOsWindow)
            return;

        Rect2i space;
        if (!component) {
            InterfacesVector size = mOsWindow->renderSize();
            space = {
                { 0, 0 }, { size.x, size.y }
            };
        } else
            space = component->getChildClientSpace();

        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            if (component) {
                if (component == comp.get()) {
                    component = nullptr;
                }
            } else {
                comp->onResize(space);
                space = comp->getChildClientSpace();
            }
        }
    }

    /**
     * @brief 
     * @param arg 
     * @return 
    */
    bool MainWindow::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            if (comp->injectKeyPress(arg))
                return true;
        }
        return false;
    }

    /**
     * @brief 
     * @param arg 
     * @return 
    */
    bool MainWindow::injectKeyRelease(const Input::KeyEventArgs &arg)
    {
        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            if (comp->injectKeyRelease(arg))
                return true;
        }
        return false;
    }

    /**
     * @brief 
     * @param arg 
     * @return 
    */
    bool MainWindow::injectPointerPress(const Input::PointerEventArgs &arg)
    {

        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            if (comp->injectPointerPress(arg))
                return true;
        }

        return false;
    }

    /**
     * @brief 
     * @param arg 
     * @return 
    */
    bool MainWindow::injectPointerRelease(const Input::PointerEventArgs &arg)
    {

        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            if (comp->injectPointerRelease(arg))
                return true;
        }

        return false;
    }

    /**
     * @brief 
     * @param arg 
     * @return 
    */
    bool MainWindow::injectPointerMove(const Input::PointerEventArgs &arg)
    {

        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            if (comp->injectPointerMove(arg))
                return true;
        }

        return false;
    }

    /**
     * @brief 
     * @param arg 
     * @return 
    */
    bool MainWindow::injectAxisEvent(const Input::AxisEventArgs &arg)
    {
        for (const std::unique_ptr<MainWindowComponentBase> &comp : reverseIt(components())) {
            if (comp->injectAxisEvent(arg))
                return true;
        }

        return false;
    }

    /**
     * @brief 
    */
    void MainWindow::onClose()
    {
        storeWindowData();
        mOsWindow = nullptr;
        mTaskQueue.stop();
    }

    /**
     * @brief 
    */
    void MainWindow::onRepaint()
    {
    }

    /**
     * @brief 
     * @param size 
    */
    void MainWindow::onResize(const InterfacesVector &size)
    {
        mRenderWindow->resize({ size.x, size.y });
        applyClientSpaceResize();
    }

    /**
     * @brief 
    */
    void MainWindow::storeWindowData()
    {
        Filesystem::FileManager mgr { "MainWindow-Layout" };

        if (Serialize::FormattedSerializeStream out = mgr.openWrite(Filesystem::appDataPath() / "mainwindow.ini", std::make_unique<Serialize::IniFormatter>())) {
            write(out, mOsWindow->data(), "data");
        }
    }

    void MainWindow::sTestScreens(size_t n)
    {
        int rows = ceil(sqrt(n + 0.25f) - 0.5f);
        int cols = (n - 1) / rows + 1;

        InterfacesVector monitorSize = listMonitors().front().mSize;

        InterfacesVector size = { monitorSize.x / cols, monitorSize.y / rows };

        std::unique_lock lock { sTestPositionMutex };

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (row * cols + col == n)
                    break;
                sTestPositions.emplace(WindowData { { col * size.x, row * size.y }, size });
            }
        }
    }

}
}
