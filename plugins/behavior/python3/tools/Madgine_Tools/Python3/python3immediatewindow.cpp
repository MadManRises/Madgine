#include "python3toolslib.h"

#include "python3immediatewindow.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Python3/python3debugger.h"
#include "Python3/python3env.h"
#include "Python3/util/python3lock.h"

#include "Generic/execution/execution.h"

#include "Madgine/debug/debugger.h"

#include "Madgine_Tools/texteditor/texteditor.h"

#include "Generic/execution/algorithm.h"

#include "Madgine_Tools/texteditor/textdocument.h"

METATABLE_BEGIN_BASE(Engine::Tools::Python3ImmediateWindow, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::Python3ImmediateWindow)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::Python3ImmediateWindow, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::Python3ImmediateWindow)

UNIQUECOMPONENT(Engine::Tools::Python3ImmediateWindow)

namespace Engine {
namespace Tools {

    Python3ImmediateWindow::Python3ImmediateWindow(ImRoot &root)
        : Tool<Python3ImmediateWindow>(root)
    {
    }

    std::string_view Python3ImmediateWindow::key() const
    {
        return "Python3ImmediateWindow";
    }

    Threading::Task<bool> Python3ImmediateWindow::init()
    {
        Debug::Debugger::getSingleton().addListener(this);

        mEnv = &Scripting::Python3::Python3Environment::getSingleton();

        co_return co_await ToolBase::init();
    }

    Threading::Task<void> Python3ImmediateWindow::finalize()
    {
        Debug::Debugger::getSingleton().removeListener(this);

        co_await ToolBase::finalize();
    }

    void Python3ImmediateWindow::renderMenu()
    {
        ToolBase::renderMenu();
    }

    void Python3ImmediateWindow::render()
    {
        if (ImGui::Begin("Python3ImmediateWindow", &mVisible)) {

            if (!mPrompt)
                mPrompt = std::make_unique<InteractivePrompt>(&getTool<TextEditor>(), this);

            mPrompt->render();
        }
        ImGui::End();
    }

    bool Python3ImmediateWindow::pass(Debug::ContextInfo &context)
    {

        const Scripting::Python3::Python3DebugLocation *location = dynamic_cast<const Scripting::Python3::Python3DebugLocation *>(context.getLocation());
        if (location) {
            const Filesystem::Path &path = location->file();

            if (context.mSingleStepping) {
                if (!path.empty()) {
                    TextDocument &doc = getTool<TextEditor>().openDocument(path);
                    doc.goToLine(location->lineNr());
                } else {
                    ImGui::MakeTabVisible("Python3ImmediateWindow");
                }
                return false;
            } else {
                if (!path.empty()) {
                    TextDocument *doc = getTool<TextEditor>().getDocument(path);
                    if (doc && doc->hasBreakpoint(location->lineNr())) {
                        doc->goToLine(location->lineNr());
                        return false;
                    }
                } 
            }
        }

        return true;
    }

    bool Python3ImmediateWindow::interpret(std::string_view command)
    {
        Execution::detach(mEnv->execute(command, [this](std::string_view text) {
            mPrompt->append(text);
        }) | Execution::finally([this]() { mPrompt->resume(); }));
        return false;
    }

}
}
