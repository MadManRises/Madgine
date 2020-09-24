#pragma once

#include "Madgine/threading/framelistener.h"
#include "handlercollector.h"

#include "Madgine/window/mainwindowcomponentcollector.h"
#include "Madgine/window/mainwindowcomponent.h"

#include "Modules/madgineobject/madgineobjectobserver.h"

#include "Modules/keyvalue/keyvalueset.h"

#include "Modules/serialize/container/serializablecontainer.h"


#include "Modules/uniquecomponent/uniquecomponentcontainer.h"

#include "Modules/math/vector2.h"


namespace Engine {
namespace UI {
    struct MADGINE_UI_EXPORT UIManager : Window::MainWindowComponent<UIManager>,
                                         Threading::FrameListener {
        SERIALIZABLEUNIT;

        UIManager(Window::MainWindow &window);
        UIManager(const UIManager &) = delete;

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
            return static_cast<T &>(getGuiHandler(Engine::component_index<T>(), init));
        }

        GuiHandlerBase &getGuiHandler(size_t i, bool = true);

        template <typename T>
        T &getGameHandler(bool init = true)
        {
            return static_cast<T &>(getGameHandler(Engine::component_index<T>(), init));
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