#pragma once

namespace Engine {
namespace Serialize {

	enum class StreamMode {
        READ,
        WRITE
    };

    struct META_EXPORT SerializeStreamData {
    public:
        SerializeStreamData(std::unique_ptr<Formatter> format);
        SerializeStreamData(SerializeStreamData &&) = delete;
        SerializeStreamData(std::unique_ptr<Formatter> format, SerializeManager &mgr, ParticipantId id);
        virtual ~SerializeStreamData();

        void setManager(SerializeManager *mgr);
        SerializeManager *manager();
        bool isMaster(StreamMode mode);

        ParticipantId id() const;
        void setId(ParticipantId id);

        Formatter &format() const;

        SerializableUnitList &serializableList();
        friend struct SerializableListHolder;
        void startSerializableRead(SerializableListHolder *list);

        SerializableUnitMap &serializableMap();
        friend struct SerializableMapHolder;
        void startSerializableWrite(SerializableMapHolder *map);

    private:
        SerializeManager *mManager = nullptr;
        ParticipantId mId = 0;
        std::unique_ptr<Formatter> mFormatter;

        union {
            SerializableUnitMap *mSerializableMap = nullptr;
            SerializableUnitList *mSerializableList;
        };
    };

}
}