#include "clickbricklib.h"

#include "gameoverhandler.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Madgine/widgets/widget.h"

#include "Madgine/widgets/label.h"

#include "gamemanager.h"

#include "Meta/serialize/serializetable_impl.h"

UNIQUECOMPONENT(ClickBrick::GameOverHandler)

METATABLE_BEGIN_BASE(ClickBrick::GameOverHandler, Engine::Input::GuiHandlerBase)
METATABLE_END(ClickBrick::GameOverHandler)

SERIALIZETABLE_INHERIT_BEGIN(ClickBrick::GameOverHandler, Engine::Input::GuiHandlerBase)
SERIALIZETABLE_END(ClickBrick::GameOverHandler)

namespace ClickBrick {

    GameOverHandler::GameOverHandler(Engine::Input::UIManager &ui)
    : Engine::Input::GuiHandler<GameOverHandler>(ui, Engine::Input::GuiHandlerBase::WindowType::MODAL_OVERLAY)
    {
    }

    std::string_view GameOverHandler::key() const
    {
        return "GameOverHandler";
    }

    void GameOverHandler::setWidget(Engine::Widgets::WidgetBase *w)
    {
        Engine::Input::GuiHandlerBase::setWidget(w);
        if (widget()) {
            widget()->pointerUpEvent().connect(&GameOverHandler::restartGame, this, &mConStore);
            mScoreLabel = widget()->getChildRecursive<Engine::Widgets::Label>("ScoreLabel");
        } else {
            mScoreLabel = nullptr;
        }
    }

    void GameOverHandler::restartGame()
    {
        getGameHandler<GameManager>().start();
        close();
    }

    void GameOverHandler::setScore(int score)
    {
        mScoreLabel->mText = "Your Score: " + std::to_string(score);
    }

}