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

#include "Modules/threading/workgroup.h"

#include "../imguiicons.h"

#include "../renderer/imroot.h"

UNIQUECOMPONENT(Engine::Tools::LogViewer);

METATABLE_BEGIN_BASE(Engine::Tools::LogViewer, Engine::Tools::ToolBase)
METATABLE_END(Engine::Tools::LogViewer)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::LogViewer, Engine::Tools::ToolBase)
FIELD(mMsgFilters)
SERIALIZETABLE_END(Engine::Tools::LogViewer)

namespace Engine {
namespace Tools {

    static constexpr size_t sLookupStep = 1;
    static constexpr std::array<const char *, 4> sIcons { "D", IMGUI_ICON_INFO, IMGUI_ICON_WARNING, IMGUI_ICON_ERROR };

    LogViewer::LogViewer(ImRoot &root)
        : Tool<LogViewer>(root, true)
        , mWorkgroup(&Threading::WorkGroup::self())
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
            ImGui::SetWindowDockingDir(mRoot.dockSpaceId(), ImGuiDir_Down, 0.3f, true, ImGuiCond_FirstUseEver);

            int mTotalMsgCount = 0;
            bool filterChanged = false;

            for (Util::MessageType type : Util::MessageType::values()) {
                filterChanged |= ImGui::Checkbox(sIcons[type], &mMsgFilters[type]);
                ImGui::SameLine();

                if (mMsgFilters[type])
                    mTotalMsgCount += mMsgCounts[type];
            }
            filterChanged |= ImGui::InputText("Filter", &mMessageWordFilter);

            std::lock_guard guard { mMutex };

            static bool first = true;
            if (filterChanged || first) {
                first = false;
                mFilteredMsgCount = 0;
                mLookup.clear();
                size_t i = 0;
                for (LogEntry &entry : mEntries) {
                    if (filter(entry))
                        addFilteredMessage(i);
                    ++i;
                }
            }

            auto end = mEntries.end();
            auto begin = mEntries.size() <= 100 ? mEntries.begin() : std::prev(end, 100);

            if (ImGui::BeginTable("Messages", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Hideable | ImGuiTableFlags_SizingFixedFit)) {

                ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_NoHide);
                ImGui::TableSetupColumn("Source");
                ImGui::TableSetupColumn("Line");
                ImGui::TableSetupScrollFreeze(0, 1);
                ImGui::TableHeadersRow();

                ImGuiListClipper clipper(mFilteredMsgCount);
                while (clipper.Step()) {
                    if (clipper.DisplayStart == clipper.DisplayEnd)
                        continue;
                    size_t skip = clipper.DisplayStart % sLookupStep;
                    size_t count = clipper.DisplayEnd - clipper.DisplayStart;
                    for (auto it = mEntries.begin() + mLookup[clipper.DisplayStart / sLookupStep]; it != mEntries.end() && count > 0; ++it) {
                        const LogEntry &entry = *it;
                        if (filter(entry)) {
                            if (skip > 0) {
                                --skip;
                            } else {
                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                ImGui::Text("%s", sIcons[entry.mType]);
                                ImGui::TableNextColumn();
                                ImGui::Selectable(entry.mMsg.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);
                                ImGui::TableNextColumn();
                                if (entry.mFile)
                                    ImGui::Text("%s", entry.mFile);
                                ImGui::TableNextColumn();
                                if (entry.mFile)
                                    ImGui::Text("%zu", entry.mLine);
                                --count;
                            }
                        }                        
                    }
                }
                ImGui::EndTable();
            }
        }
        ImGui::End();
    }

    void LogViewer::renderStatus()
    {
        for (Util::MessageType type : Util::MessageType::values()) {
            ImGui::Text("%s %d", sIcons[type], mMsgCounts[type]);
        }
    }

    void LogViewer::messageLogged(std::string_view message, Util::MessageType lml, const char *file, size_t line, Util::Log *log)
    {
        if (mWorkgroup != &Threading::WorkGroup::self())
            return;
        ++mMsgCounts[lml];
        std::lock_guard guard { mMutex };
        if (filter(mEntries.emplace_back(std::string { message }, lml, file, line)))
            addFilteredMessage(mEntries.size() - 1);
    }

    std::string_view LogViewer::key() const
    {
        return "LogViewer";
    }

    bool LogViewer::filter(const LogEntry &entry)
    {
        return mMsgFilters[entry.mType] && (mMessageWordFilter.empty() || StringUtil::contains(entry.mMsg, mMessageWordFilter));
    }

    void LogViewer::addFilteredMessage(size_t index)
    {
        if (mFilteredMsgCount % sLookupStep == 0) {
            mLookup.push_back(index);
        }
        ++mFilteredMsgCount;
    }

}
}
