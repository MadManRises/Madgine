#pragma once

#include "handlercollector.h"

#include "Madgine/window/mainwindowcomponent.h"
#include "Madgine/window/mainwindowcomponentcollector.h"

#include "Generic/keyvalue.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Meta/math/vector2.h"

#include "Generic/intervalclock.h"

namespace Engine {
namespace UI {
    struct MADGINE_UI_EXPORT UIManager : Window::MainWindowComponent<UIManager> {
        SERIALIZABLEUNIT(UIManager);

        UIManager(Window::MainWindow &window);
        UIManager(const UIManager &) = delete;

        ~UIManager();

        void clear();

        void hideCursor(bool keep = true);
        void showCursor();
        bool isCursorVisible() const;

        void update();
        void fixedUpdate();

        //Scene::ContextMask currentContext();

        std::set<GameHandlerBase *> getGameHandlers();
        std::set<GuiHandlerBase *> getGuiHandlers();

        static const constexpr int sMaxInitOrder = 4;

        std::string_view key() const override;

        template <typename T>
        T &getGuiHandler()
        {
            return static_cast<T &>(getGuiHandler(Engine::component_index<T>()));
        }

        GuiHandlerBase &getGuiHandler(size_t i);

        template <typename T>
        T &getGameHandler()
        {
            return static_cast<T &>(getGameHandler(Engine::component_index<T>()));
        }

        GameHandlerBase &getGameHandler(size_t i);

        GuiHandlerContainer<std::set<Placeholder<0>, KeyCompare<Placeholder<0>>>> mGuiHandlers;
        GameHandlerContainer<std::set<Placeholder<0>, KeyCompare<Placeholder<0>>>> mGameHandlers;

    protected:
        Threading::Task<bool> init() override;
        Threading::Task<void> finalize() override;

    private:
        Vector2 mKeptCursorPosition;
        bool mKeepingCursorPos = false;

        IntervalClock<std::chrono::steady_clock> mFrameClock;
    };
}
}

RegisterType(Engine::UI::UIManager);