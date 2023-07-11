#include "../../scenelib.h"
#include "entitycomponentbase.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/serializetable_impl.h"

#include "entity.h"
#include "../scenemanager.h"
#include "entitycomponentlistbase.h"

METATABLE_BEGIN(Engine::Scene::Entity::EntityComponentBase)
METATABLE_END(Engine::Scene::Entity::EntityComponentBase)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::SerializableEntityComponentBase)
SERIALIZETABLE_END(Engine::Scene::Entity::SerializableEntityComponentBase)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::SyncableEntityComponentBase)
SERIALIZETABLE_END(Engine::Scene::Entity::SyncableEntityComponentBase)

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityComponentBase::EntityComponentBase(const ObjectPtr &initTable)
        {
        }

        SerializableEntityComponentBase::SerializableEntityComponentBase(const ObjectPtr &initTable)
            : EntityComponentBase(initTable)
        {
        }

        SyncableEntityComponentBase::SyncableEntityComponentBase(const ObjectPtr &initTable)
            : EntityComponentBase(initTable)
        {
            //This should never be thrown. Provide an Entity* to your component.
            throw 0;
        }

        SyncableEntityComponentBase::SyncableEntityComponentBase(const ObjectPtr &initTable, Entity *entity)
            : EntityComponentBase(initTable)
            , mEntity(entity)
        {
        }

        bool SyncableEntityComponentBase::isMaster() const
        {
            return mEntity->isMaster();
        }

        void SyncableEntityComponentBase::writeAction(OffsetPtr offset, size_t componentIndex, void *data, Serialize::ParticipantId answerTarget, Serialize::MessageId answerId, const std::set<Serialize::ParticipantId> &targets) const
        {            
            EntityComponentActionPayload payload;
            payload.mComponentIndex = componentIndex;            
            payload.mOffset = offset;
            payload.mComponent = this;
            payload.mData = data;

            std::set<std::reference_wrapper<Serialize::FormattedBufferedStream>, Serialize::CompareStreamId> streams = mEntity->getMasterActionMessageTargets(answerTarget, answerId, targets);
            mEntity->serializeType()->writeAction(mEntity, 1, streams, &payload);
            for (Serialize::FormattedBufferedStream &stream : streams)
                stream.endMessageWrite();
        }

        void Engine::Scene::Entity::SyncableEntityComponentBase::writeRequest(OffsetPtr offset, void *data, Serialize::ParticipantId requester, Serialize::MessageId requesterTransactionId, Serialize::GenericMessageReceiver receiver) const
        {
            throw 0;
        }

        Serialize::FormattedBufferedStream &Engine::Scene::Entity::SyncableEntityComponentBase::getSlaveRequestMessageTarget(Serialize::ParticipantId requester, Serialize::MessageId requesterTransactionId, Serialize::GenericMessageReceiver receiver) const
        {
            throw 0;
        }

    }
}
}
