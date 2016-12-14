#pragma once

#include "serializable.h"
#include "serializablebase.h"

namespace Engine {
namespace Serialize {

class MADGINE_EXPORT SerializableUnit : public Serializable
{
public:
	SerializableUnit(TopLevelSerializableUnit *topLevel = 0, TopLevelMadgineObject type = NO_TOP_LEVEL);
	virtual ~SerializableUnit();	

	std::list<SerializeOutStream *> getMasterMessageTargets(bool isAction);
	SerializeOutStream *getSlaveMessageTarget();

	virtual void writeCreationData(SerializeOutStream &out) const;
	virtual void writeState(SerializeOutStream &out) const;

	virtual void readState(SerializeInStream &in);
	void readAction(SerializeInStream &in);
	void readRequest(SerializeInStream &in);

	int addObservable(Observable* val);
	void addSerializableValue(Serializable *val);

	TopLevelSerializableUnit *topLevel();


	InvPtr masterId();
	void setMasterId(InvPtr id);

	virtual void applySerializableMap(const std::map <InvPtr, SerializableUnit*> &map) override;

protected:
	virtual void writeHeader(SerializeOutStream &out, bool isAction);	

	

private:
	std::vector<Observable*> mObservedValues;
	std::vector<Serializable*> mStateValues;

	TopLevelSerializableUnit *mTopLevel;

	TopLevelMadgineObject mObject;

	InvPtr mMasterId;
};

} // namespace Serialize
} // namespace Core

