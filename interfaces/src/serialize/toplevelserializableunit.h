#pragma once

#include "serializableunit.h"

namespace Engine {
namespace Serialize {

class INTERFACES_EXPORT TopLevelSerializableUnitBase : public SerializableUnitBase
{
public:
	TopLevelSerializableUnitBase(size_t staticId = 0);
	TopLevelSerializableUnitBase(const TopLevelSerializableUnitBase &other);
	TopLevelSerializableUnitBase(TopLevelSerializableUnitBase &&other);
	virtual ~TopLevelSerializableUnitBase();

	void copyStaticSlaveId(const TopLevelSerializableUnitBase &other);

	std::list<BufferedOutStream *> getMasterMessageTargets(SerializableUnitBase *unit, MessageType type, const std::list<ParticipantId> &targets);
	BufferedOutStream *getSlaveMessageTarget(SerializableUnitBase *unit);

	const std::list<SerializeManager*> &getMasterManagers();
	SerializeManager *getSlaveManager();

	bool addManager(SerializeManager *mgr);
	void removeManager(SerializeManager *mgr);

	bool updateManagerType(SerializeManager *mgr, bool isMaster);

	bool isMaster();

	static ParticipantId getLocalMasterParticipantId();
	ParticipantId getSlaveParticipantId();

	void setStaticSlaveId(size_t staticId);
	void initSlaveId();

	virtual size_t readId(SerializeInStream &in) override;

	virtual bool init();

private:
	std::list<SerializeManager*> mMasterManagers;
	SerializeManager *mSlaveManager;
	size_t mStaticSlaveId;

};

template <class T>
class TopLevelSerializableUnit : public TopLevelSerializableUnitBase {
	using TopLevelSerializableUnitBase::TopLevelSerializableUnitBase;

	virtual size_t getSize() const override {
		return sizeof(T);
	}
};


} // namespace Serialize
} // namespace Core

