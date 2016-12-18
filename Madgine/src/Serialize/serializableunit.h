#pragma once

#include "serializable.h"
#include "serializablebase.h"

namespace Engine {
namespace Serialize {

class MADGINE_EXPORT SerializableUnit
{
public:
	SerializableUnit(TopLevelSerializableUnit *topLevel = 0);
	virtual ~SerializableUnit();	

	std::list<BufferedOutStream *> getMasterMessageTargets(bool isAction);
	BufferedOutStream *getSlaveMessageTarget();

	virtual void writeCreationData(SerializeOutStream &out) const;
	void writeState(SerializeOutStream &out) const;

	void readState(SerializeInStream &in);
	void readAction(SerializeInStream &in);
	void readRequest(BufferedInOutStream &in);

	int addObservable(Observable* val);
	void addSerializableValue(Serializable *val);

	TopLevelSerializableUnit *topLevel();

	void clearMasterId();

	/*InvPtr masterId();
	void setMasterId(InvPtr id);*/

	void applySerializableMap(const std::map <InvPtr, SerializableUnit*> &map);

	void writeHeader(SerializeOutStream &out, bool isAction);	

	virtual TopLevelMadgineObject type();
	

private:
	std::vector<Observable*> mObservedValues;
	std::vector<Serializable*> mStateValues;

	TopLevelSerializableUnit *mTopLevel;

	InvPtr mMasterId;
};

} // namespace Serialize
} // namespace Core

