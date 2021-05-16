#include "clickbricklib.h"

#include "gameoverhandler.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "gamehandler.h"

#include "Madgine/widgets/widget.h"

#include "Madgine/widgets/label.h"

#include "gamemanager.h"

#include "Meta/serialize/serializetable_impl.h"

UNIQUECOMPONENT(ClickBrick::UI::GameOverHandler)

METATABLE_BEGIN_BASE(ClickBrick::UI::GameOverHandler, Engine::UI::GuiHandlerBase)
METATABLE_END(ClickBrick::UI::GameOverHandler)

SERIALIZETABLE_INHERIT_BEGIN(ClickBrick::UI::GameOverHandler, Engine::UI::GuiHandlerBase)
SERIALIZETABLE_END(ClickBrick::UI::GameOverHandler)

namespace ClickBrick {
namespace UI {

    GameOverHandler::GameOverHandler(Engine::UI::UIManager &ui)
        : Engine::UI::GuiHandler<GameOverHandler>(ui, Engine::UI::GuiHandlerBase::WindowType::MODAL_OVERLAY)
        , mScoreLabel(this, "ScoreLabel")
    {
    }

    std::string_view GameOverHandler::key() const
    {
        return "GameOverHandler";
    }

    void GameOverHandler::setWidget(Engine::Widgets::WidgetBase *w)
    {
        Engine::UI::GuiHandlerBase::setWidget(w);
        if (widget()) {
            widget()->pointerUpEvent().connect(&GameOverHandler::restartGame, this, &mConStore);
        }
    }

    void GameOverHandler::restartGame()
    {
        getGameHandler<GameManager>().start();
        close();
    }

    void ClickBrick::UI::GameOverHandler::setScore(int score)
    {
        mScoreLabel->mText = "Your Score: " + std::to_string(score);
    }

}
}