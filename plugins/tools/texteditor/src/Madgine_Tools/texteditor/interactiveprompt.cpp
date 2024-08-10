#include "../texteditorlib.h"

#include "interactiveprompt.h"

#include "imgui/imgui.h"

#include "Madgine/resources/resourcemanager.h"

#include "texteditor.h"

#include <zep/mode_repl.h>

namespace Engine {
namespace Tools {

    InteractivePrompt::InteractivePrompt(TextEditor *editor, Interpreter *interpreter)
        : mEditor(Resources::ResourceManager::getSingleton().findResourceFile("repl.cfg").str(), sPixelScale())
        , mInterpreter(interpreter)
    {
        auto &display = static_cast<Zep::ZepDisplay_ImGui &>(mEditor.GetDisplay());

        ImFont *font = editor->font();
        int fontPixelHeight = editor->fontPixelHeight();

        display.SetFont(Zep::ZepTextType::UI, std::make_shared<Zep::ZepFont_ImGui>(display, font, fontPixelHeight));
        display.SetFont(Zep::ZepTextType::Text, std::make_shared<Zep::ZepFont_ImGui>(display, font, fontPixelHeight));
        display.SetFont(Zep::ZepTextType::Heading1, std::make_shared<Zep::ZepFont_ImGui>(display, font, int(fontPixelHeight * 1.75)));
        display.SetFont(Zep::ZepTextType::Heading2, std::make_shared<Zep::ZepFont_ImGui>(display, font, int(fontPixelHeight * 1.5)));
        display.SetFont(Zep::ZepTextType::Heading3, std::make_shared<Zep::ZepFont_ImGui>(display, font, int(fontPixelHeight * 1.25)));

        mBuffer = mEditor.InitWithText(std::string { mInterpreter->name() }, "");
        mBuffer->SetBufferType(Zep::BufferType::Repl);
        mBuffer->SetPostKeyNotifier([this](uint32_t key, uint32_t modifier) {
            return handleKeyPress(key, modifier);
        });
        mWindow = mEditor.GetActiveWindow();

        mEditor.SetGlobalMode(Zep::ZepMode_Standard::StaticName());

        mEditor.GetConfig().autoHideCommandRegion = false;

        prompt();
    }

    void InteractivePrompt::render()
    {
        auto min = ImGui::GetCursorScreenPos();
        auto max = ImGui::GetContentRegionAvail();
        max.x = std::max(1.0f, max.x);
        max.y = std::max(1.0f, max.y);

        // Fill the window
        max.x = min.x + max.x;
        max.y = min.y + max.y;
        mEditor.SetDisplayRegion(Zep::NVec2f { min.x, min.y }, Zep::NVec2f { max.x, max.y });

        mEditor.Display();

        if (ImGui::IsWindowFocused())
            mEditor.HandleInput();
    }

    bool InteractivePrompt::handleKeyPress(uint32_t key, uint32_t modifier)
    {
        if (key == Zep::ExtKeys::RETURN && modifier == 0) {
            Zep::ChangeRecord record;
            auto &buffer = mWindow->GetBuffer();
            std::string str = std::string(buffer.GetWorkingBuffer().begin() + mStartLocation.Index(), buffer.GetWorkingBuffer().end());
            if (str.size() <= 1) {
                moveToEnd();
                buffer.GetMode()->SwitchMode(Zep::EditorMode::Insert);
                return false;
            }

            bool newline = true;
            int pos = 0;
            while (pos < str.size()) {
                if (str[pos] == '\n')
                    newline = true;
                else if (newline) {
                    if (str.find(">> ", pos) == pos) {
                        str.erase(pos, 3);
                        continue;
                    } else if (str.find("> ", pos) == pos) {
                        str.erase(pos, 2);
                        continue;
                    }
                    newline = false;
                }
                pos++;
            }

            if (mInterpreter->interpret(str)) {
                resume();
            }
            return true;
        }

        return false;
    }

    void InteractivePrompt::prompt()
    {
        // TODO: Repl broken, but when not, need to consider undo
        Zep::ChangeRecord changeRecord;
        mBuffer->Insert(mBuffer->End(), ">> ", changeRecord);
        moveToEnd();
    }

    void InteractivePrompt::moveToEnd()
    {
        mWindow->SetBufferCursor(mBuffer->End());
        mStartLocation = mWindow->GetBufferCursor();
    }

    void InteractivePrompt::resume()
    {
        prompt();
    }

    std::string InteractivePrompt::getName()
    {
        return std::string{ mInterpreter->name() };
    }

    void InteractivePrompt::log(std::string_view msg, Engine::Log::MessageType lvl, const char *file, size_t line)
    {
        Zep::ChangeRecord changeRecord;
        mBuffer->Insert(mBuffer->End(), std::string { msg } + "\n", changeRecord);
    }

}
}