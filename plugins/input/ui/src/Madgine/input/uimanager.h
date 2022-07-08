#pragma once

#include "handlercollector.h"

#include "Generic/keyvalue.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Meta/math/vector2.h"

#include "Generic/intervalclock.h"

#include "madgineobject/madgineobject.h"

namespace Engine {
namespace Input {
    struct MADGINE_UI_EXPORT UIManager : MadgineObject<UIManager> {

        UIManager(App::Application &app, Window::MainWindow &window);
        UIManager(const UIManager &) = delete;

        ~UIManager();

        void clear();

        void hideCursor(bool keep = true);
        void showCursor();
        bool isCursorVisible() const;

        void updateRender();
        void fixedUpdateRender();
        void updateApp();

        //Scene::ContextMask currentContext();

        std::set<GameHandlerBase *> getGameHandlers();
        std::set<GuiHandlerBase *> getGuiHandlers();

        static const constexpr int sMaxInitOrder = 4;

        std::string_view key() const;

        template <typename T>
        T &getGuiHandler()
        {
            return static_cast<T &>(getGuiHandler(Engine::component_index<T>()));
        }

        GuiHandlerBase &getGuiHandler(size_t i);

        Threading::TaskQueue *viewTaskQueue() const;

        Threading::TaskQueue *modelTaskQueue() const;

        template <typename T>
        T &getGameHandler()
        {
            return static_cast<T &>(getGameHandler(Engine::component_index<T>()));
        }

        GameHandlerBase &getGameHandler(size_t i);

        Threading::Task<bool> init();
        Threading::Task<void> finalize();

        App::Application &app() const;
        Window::MainWindow &window() const;

        void onUpdate();

    private:
        App::Application &mApp;
        Window::MainWindow &mWindow;

    public:
        GuiHandlerContainer<std::set<Placeholder<0>, KeyCompare<Placeholder<0>>>> mGuiHandlers;
        GameHandlerContainer<std::set<Placeholder<0>, KeyCompare<Placeholder<0>>>> mGameHandlers;

    private:
        Vector2 mKeptCursorPosition;
        bool mKeepingCursorPos = false;

        IntervalClock<std::chrono::steady_clock> mFrameClock, mAppClock;
    };
}
}

RegisterType(Engine::Input::UIManager);