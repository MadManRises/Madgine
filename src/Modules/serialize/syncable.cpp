#include "../moduleslib.h"

#include "syncable.h"
#include "streams/bufferedstream.h"
#include "toplevelserializableunit.h"

namespace Engine {
namespace Serialize {

    std::set<BufferedOutStream *, CompareStreamId> SyncableBase::getMasterActionMessageTargets(const SerializableUnitBase *parent, size_t offset,
        const std::set<ParticipantId> &targets) const
    {
        std::set<BufferedOutStream *, CompareStreamId> result = parent->getMasterMessageTargets();
        if (targets.empty()) {
            for (BufferedOutStream *out : result) {
                out->beginMessage(parent, ACTION);
                *out << offset;
            }
        } else {
            auto it1 = result.begin();
            auto it2 = targets.begin();
            while (it1 != result.end() && it2 != targets.end()) {
                BufferedOutStream *out = *it1;
                while (*it2 < out->id()) {
                    LOG_WARNING("Specific Target not in MessageTargetList!");
                    ++it2;
                }
                if (*it2 == out->id()) {
                    out->beginMessage(parent, ACTION);
                    *out << 0;
                    ++it2;
                    ++it1;
                } else {
                    it1 = result.erase(it1);
                }
            }
            result.erase(it1, result.end());
        }

        return result;
    }

    ParticipantId SyncableBase::participantId(const SerializableUnitBase *parent)
    {
        return parent->topLevel()->participantId();
    }

    BufferedOutStream *SyncableBase::getSlaveActionMessageTarget(const SerializableUnitBase *parent, size_t offset) const
    {
        BufferedOutStream *out = parent->getSlaveMessageTarget();
        out->beginMessage(parent, REQUEST);
        *out << offset;
        return out;
    }

    void SyncableBase::beginActionResponseMessage(const SerializableUnitBase *parent, size_t offset, BufferedOutStream *stream) const
    {
        stream->beginMessage(parent, ACTION);
        *stream << offset;
    }

    bool SyncableBase::isMaster(const SerializableUnitBase *parent) const
    {
        return !parent->isSynced() || !parent->topLevel() || parent->topLevel()->isMaster();
    }

}
}
