#pragma once

#include "Interfaces/filesystem/path.h"

#include <zep.h>
#include <zep/mode_repl.h>

namespace Engine {
namespace Tools {

    struct Interpreter {
        virtual bool interpret(std::string_view command) = 0;
    };

    struct MADGINE_TEXTEDITOR_EXPORT InteractivePrompt {

        InteractivePrompt(TextEditor *editor, Interpreter *interpreter);

        void render();

        void append(std::string_view s);

        void resume();

    protected:
        bool handleKeyPress(uint32_t key, uint32_t modifier);

        void prompt();
        void moveToEnd();

    private:
        Zep::ZepEditor_ImGui mEditor;

        Zep::ZepBuffer *mBuffer;
        Zep::ZepWindow *mWindow;
        Zep::GlyphIterator mStartLocation;

        Interpreter *mInterpreter;
    };

}
}