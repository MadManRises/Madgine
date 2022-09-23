#pragma once

#include "../toolbase.h"
#include "../toolscollector.h"

#include "Interfaces/util/loglistener.h"

namespace Engine {
namespace Tools {

    struct LogViewer : Tool<LogViewer>, Util::LogListener {
        SERIALIZABLEUNIT(LogViewer)

        LogViewer(ImRoot &root);
        ~LogViewer();

        virtual void render() override;
        virtual void renderStatus() override;

        virtual void messageLogged(std::string_view message, Util::MessageType lml, const char *file, size_t line, Util::Log *log) override;

        std::string_view key() const override;

    protected:
        struct LogEntry {
            LogEntry(std::string msg, Util::MessageType type, const char *file, size_t line)
                : mMsg(msg)
                , mType(type)
                , mFile(file)
                , mLine(line)
            {
            }

            std::string mMsg;
            Util::MessageType mType;
            const char *mFile;
            size_t mLine;
        };

        bool filter(const LogEntry &entry);

        void addFilteredMessage(size_t index);

    private:
        Threading::WorkGroup *mWorkgroup;
        std::deque<LogEntry> mEntries;
        std::array<size_t, Util::MessageType::COUNT> mMsgCounts;
        std::mutex mMutex;

        std::array<bool, Util::MessageType::COUNT> mMsgFilters;
        std::string mMessageWordFilter;
        size_t mFilteredMsgCount = 0;
        std::vector<size_t> mLookup;
    };

}
}

REGISTER_TYPE(Engine::Tools::LogViewer)