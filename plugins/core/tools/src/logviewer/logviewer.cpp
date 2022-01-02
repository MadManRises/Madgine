#include "../toolslib.h"

#include "logviewer.h"

#include "imgui/imgui.h"
#include "imgui/imguiaddons.h"

#include "Meta/serialize/serializetable_impl.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Interfaces/util/standardlog.h"

#include "Generic/coroutines/generator.h"

#include "Interfaces/filesystem/path.h"

UNIQUECOMPONENT(Engine::Tools::LogViewer);

METATABLE_BEGIN_BASE(Engine::Tools::LogViewer, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::LogViewer)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::LogViewer, Engine::Tools::ToolBase)
FIELD(mMsgFilters)
SERIALIZETABLE_END(Engine::Tools::LogViewer)

namespace Engine {
namespace Tools {

    static constexpr std::array<const char *, 4> icons { "", "\xee\xa0\x81", "\xee\xa0\x80", "\xee\xa0\x82" };

    LogViewer::LogViewer(ImRoot &root)
        : Tool<LogViewer>(root)
    {
        for (Util::MessageType type : Util::MessageType::values()) {
            mMsgCounts[type] = 0;
            mMsgFilters[type] = true;
        }
        Util::StandardLog::getSingleton().addListener(this);
    }

    LogViewer::~LogViewer()
    {
        Util::StandardLog::getSingleton().removeListener(this);
    }

    void LogViewer::render()
    {
        if (ImGui::Begin("LogViewer", &mVisible)) {

            for (Util::MessageType type : Util::MessageType::values()) {
                if (type != Util::MessageType::DEBUG_TYPE) {
                    ImGui::Checkbox(icons[type], &mMsgFilters[type]);
                    ImGui::SameLine();
                }
            }
            ImGui::Text("");

            std::lock_guard guard { mMutex };

            auto end = mEntries.end();
            auto begin = mEntries.size() <= 100 ? mEntries.begin() : std::prev(end, 100);

            if (ImGui::BeginChild("Messages")) {

                ImGui::Columns(3);
                if (!mOnce) {
                    ImGui::SetColumnWidth(0, 24);
                    mOnce = true;
                }

                for (const LogEntry &entry : mEntries) {
                    if (mMsgFilters[entry.mType]) {
                        ImGui::Text("%s", icons[entry.mType]);
                        ImGui::NextColumn();
                        ImGui::Selectable(entry.mMsg.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
                        ImGui::NextColumn();
                        if (entry.mFile)
                            ImGui::Text("%s", entry.mFile);
                        ImGui::NextColumn();
                        ImGui::Separator();
                    }
                }
                ImGui::EndChild();
            }
        }
        ImGui::End();
    }

    void LogViewer::renderStatus()
    {
        for (Util::MessageType type : Util::MessageType::values()) {
            if (type != Util::MessageType::DEBUG_TYPE) {

                ImGui::Text("%s %d", icons[type], mMsgCounts[type]);
            }
        }
    }

    void LogViewer::messageLogged(std::string_view message, Util::MessageType lml, const char *file, Util::Log *log)
    {
        ++mMsgCounts[lml];
        if (lml != Util::MessageType::DEBUG_TYPE) {
            std::lock_guard guard { mMutex };
            mEntries.push_back({ std::string { message }, lml, file });
        }
    }

    std::string_view LogViewer::key() const
    {
        return "LogViewer";
    }

}
}
