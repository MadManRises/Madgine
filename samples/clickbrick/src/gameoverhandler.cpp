#include "clickbricklib.h"

#include "gameoverhandler.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "gamehandler.h"

#include "Madgine/widgets/widget.h"

#include "Madgine/widgets/label.h"

#include "gamemanager.h"

UNIQUECOMPONENT(ClickBrick::UI::GameOverHandler)



METATABLE_BEGIN_BASE(ClickBrick::UI::GameOverHandler, Engine::UI::GuiHandlerBase)
METATABLE_END(ClickBrick::UI::GameOverHandler)

namespace ClickBrick
{
    namespace UI {

        GameOverHandler::GameOverHandler(Engine::UI::UIManager &ui)
            : Engine::UI::GuiHandler<GameOverHandler>(ui, Engine::UI::GuiHandlerBase::WindowType::MODAL_OVERLAY)
            , mRestartGameSlot(this)
            , mScoreLabel(this, "ScoreLabel")
        {
        }

        std::string_view GameOverHandler::key() const
        {
            return "GameOverHandler";
        }

        void GameOverHandler::setWidget(Engine::Widgets::WidgetBase *w)
        {
            if (widget()) {
                mRestartGameSlot.disconnectAll();
            }
            Engine::UI::GuiHandlerBase::setWidget(w);
            if (widget()) {
                widget()->pointerUpEvent().connect(mRestartGameSlot);
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