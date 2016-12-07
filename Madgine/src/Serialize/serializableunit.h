#pragma once

#include "serializable.h"

namespace Engine {
namespace Serialize {

class MADGINE_EXPORT SerializableUnit : public Serializable
{
public:
	SerializableUnit();
	virtual ~SerializableUnit();

	void setMasterManager(SerializeManager *mgr);

	std::list<SerializeOutStream *> getMessageTargets(bool isAction);

	virtual void writeCreationData(SerializeOutStream &out) const;
	virtual void writeState(SerializeOutStream &out) const;

	virtual void readState(SerializeInStream &in);
	void readAction(SerializeInStream &in);
	void readRequest(SerializeInStream &in);

	int addObservable(Observable* val);
	void addSerializableValue(Serializable *val);	

protected:
	void writeHeader(SerializeOutStream &out, bool isAction);	
	virtual TopLevelMadgineObject topLevelId();

private:
	std::vector<Observable*> mObservedValues;
	std::vector<Serializable*> mStateValues;

	SerializeManager *mMasterManager;
};

} // namespace Serialize
} // namespace Core

