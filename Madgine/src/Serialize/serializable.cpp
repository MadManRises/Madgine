#include "madginelib.h"

#include "serializable.h"

#include "serializableunit.h"

#include "Streams\bufferedstream.h"

namespace Engine {
		namespace Serialize {

			Serializable::Serializable(SerializableUnit * parent) :
				mParent(parent)
			{
				mParent->addSerializableValue(this);
			}

			Serializable::Serializable() :
				mParent(0)
			{
			}

			void Serializable::applySerializableMap(const std::map<InvPtr, SerializableUnit*>& map)
			{
			}

			std::list<BufferedOutStream*> Serializable::getMasterStateMessageTargets()
			{
				return mParent ? mParent->getMasterMessageTargets(false) : std::list<BufferedOutStream*>();
			}

			void Serializable::sendState()
			{
				for (BufferedOutStream *out : getMasterStateMessageTargets()) {
					writeState(*out);
				}
			}

			TopLevelSerializableUnit * Serializable::topLevel()
			{
				return mParent->topLevel();
			}



		}
}