#pragma once

#include "Madgine/threading/framelistener.h"
#include "gamehandler.h"
#include "guihandler.h"

#include "Modules/keyvalue/scopebase.h"

#include "Modules/keyvalue/keyvalue.h"

#include "Madgine/window/mainwindowcomponentcollector.h"
#include "Madgine/window/mainwindowcomponent.h"

#include "Modules/madgineobject/madgineobjectobserver.h"

#include "Modules/keyvalue/keyvalueset.h"

#include "Modules/serialize/container/serializablecontainer.h"

#include "Modules/serialize/container/controlledconfig.h"

#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

namespace Engine {
namespace UI {
    struct MADGINE_UI_EXPORT UIManager : Window::MainWindowComponent<UIManager>,
                                         Threading::FrameListener {
        SERIALIZABLEUNIT;

        UIManager(Window::MainWindow &window);
        ~UIManager();

        void clear();

        void hideCursor(bool keep = true);
        void showCursor();
        bool isCursorVisible() const;

        bool frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;
        bool frameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context) override;

        //Scene::ContextMask currentContext();

        Window::MainWindow &window(bool = true) const;

        std::set<GameHandlerBase *> getGameHandlers();
        std::set<GuiHandlerBase *> getGuiHandlers();

        static const constexpr int sMaxInitOrder = 4;

        std::string_view key() const override;

        template <typename T>
        T &getGuiHandler(bool init = true)
        {
            return static_cast<T &>(getGuiHandler(T::component_index(), init));
        }

        GuiHandlerBase &getGuiHandler(size_t i, bool = true);

        template <typename T>
        T &getGameHandler(bool init = true)
        {
            return static_cast<T &>(getGameHandler(T::component_index(), init));
        }

        GameHandlerBase &getGameHandler(size_t i, bool = true);

        OFFSET_CONTAINER(mGuiHandlers, GuiHandlerContainer<Serialize::SerializableContainer<KeyValueSet<Placeholder<0>>, MadgineObjectObserver, std::true_type>>);
        OFFSET_CONTAINER(mGameHandlers, GameHandlerContainer<Serialize::SerializableContainer<KeyValueSet<Placeholder<0>>, MadgineObjectObserver, std::true_type>>)

    protected:
        bool init() override;
        void finalize() override;

    private:
        Vector2 mKeptCursorPosition;
        bool mKeepingCursorPos = false;
    };
}
}

RegisterType(Engine::UI::UIManager);