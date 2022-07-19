#pragma once

#include "handlercollector.h"

#include "Generic/keyvalue.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Modules/uniquecomponent/component_index.h"

#include "Meta/math/vector2.h"

#include "Generic/intervalclock.h"

#include "Modules/threading/madgineobject.h"

namespace Engine {
namespace Input {
    struct MADGINE_UI_EXPORT UIManager : Threading::MadgineObject<UIManager> {

        UIManager(Base::Application &app, Window::MainWindow &window);
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
            return static_cast<T &>(getGuiHandler(UniqueComponent::component_index<T>()));
        }

        GuiHandlerBase &getGuiHandler(size_t i);

        Threading::TaskQueue *viewTaskQueue() const;

        Threading::TaskQueue *modelTaskQueue() const;

        template <typename T>
        T &getGameHandler()
        {
            return static_cast<T &>(getGameHandler(UniqueComponent::component_index<T>()));
        }

        GameHandlerBase &getGameHandler(size_t i);

        Threading::Task<bool> init();
        Threading::Task<void> finalize();

        Base::Application &app() const;
        Window::MainWindow &window() const;

        void onUpdate();

    private:
        Base::Application &mApp;
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

REGISTER_TYPE(Engine::Input::UIManager)