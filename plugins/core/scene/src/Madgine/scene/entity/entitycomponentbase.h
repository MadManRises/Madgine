#pragma once

#include "Meta/keyvalue/objectptr.h"
#include "Meta/serialize/hierarchy/serializableunit.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

#include "entitycomponentcontainer.h"

#include "Generic/container/compactcontainer.h"

#include "Meta/serialize/streams/pendingrequest.h"

#include "Generic/offsetptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct EntityComponentActionPayload {            
            size_t mComponentIndex;
            OffsetPtr mOffset;
            const Serialize::SerializableDataUnit *mComponent;
            void *mData;
        };

        struct MADGINE_SCENE_EXPORT EntityComponentBase {
            using Container = CompactContainer<EntityComponentContainerImpl<std::vector>, EntityComponentRelocateFunctor>;

            EntityComponentBase(const ObjectPtr &initTable);
        };

        struct MADGINE_SCENE_EXPORT SerializableEntityComponentBase : Serialize::SerializableDataUnit, EntityComponentBase {
            SerializableEntityComponentBase(const ObjectPtr &initTable = {});
        };

        struct MADGINE_SCENE_EXPORT SyncableEntityComponentBase : Serialize::SerializableUnitBase, EntityComponentBase {
            using Container = CompactContainer<std::vector<Placeholder<0>>, EntityComponentRelocateFunctor>;

            SyncableEntityComponentBase(const ObjectPtr &initTable);
            SyncableEntityComponentBase(const ObjectPtr &initTable, Entity *entity);

            bool isMaster() const;

        protected:
            void writeAction(OffsetPtr offset, size_t componentIndex, void *data, Serialize::ParticipantId answerTarget, Serialize::MessageId answerId, const std::set<Serialize::ParticipantId> &targets = {}) const;
            void writeRequest(OffsetPtr offset, void *data, Serialize::ParticipantId requester = 0, Serialize::MessageId requesterTransactionId = 0, Serialize::GenericMessageReceiver receiver = {}) const;
        
            Serialize::FormattedBufferedStream &getSlaveRequestMessageTarget(Serialize::ParticipantId requester, Serialize::MessageId requesterTransactionId, Serialize::GenericMessageReceiver receiver) const;


            
            template <typename Ty, typename... Args>
            void writeAction(Ty *field, Serialize::ParticipantId answerTarget, Serialize::MessageId answerId, Args &&...args) const
            {
                size_t componentIndex = std::remove_pointer_t<decltype(field->parent())>::component_index();
                OffsetPtr offset { static_cast<const SerializableDataUnit *>(this), field };
                typename Ty::action_payload data { std::forward<Args>(args)... };
                writeAction(offset, componentIndex, &data, answerTarget, answerId, {});
            }

            template <typename Ty, typename... Args>
            void writeRequest(Ty *field, Serialize::ParticipantId requester, Serialize::MessageId requesterTransactionId, Args &&...args) const
            {
                OffsetPtr offset { static_cast<const SerializableDataUnit *>(this), field };
                typename Ty::request_payload data { std::forward<Args>(args)... };
                writeRequest(offset, &data, requester, requesterTransactionId);
            }

            template <typename Ty, typename... Args>
            void writeRequest(Ty *field, Serialize::GenericMessageReceiver receiver, Args &&...args) const
            {
                OffsetPtr offset { static_cast<const SerializableDataUnit *>(this), field };
                typename Ty::request_payload data { std::forward<Args>(args)... };
                writeRequest(offset, &data, 0, 0, std::move(receiver));
            }



            friend auto &tag_invoke(get_component_t, auto &comp)
            {
                return comp;
            }

            friend auto &tag_invoke(get_entity_ptr_t, auto &comp)
            {
                return comp.mEntity;
            }

            template <typename T>
            friend struct Serialize::Syncable;

        private:
            NulledPtr<Entity> mEntity;
        };
    }
}
}

REGISTER_TYPE(Engine::Scene::Entity::EntityComponentBase)