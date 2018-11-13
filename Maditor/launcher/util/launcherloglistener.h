#pragma once

#include "../../shared/moduleinstance.h"
#include "Madgine/serialize/container/action.h"
#include "Madgine/util/loglistener.h"
#include "Madgine/signalslot/slot.h"

namespace Maditor {
	namespace Launcher {

class LauncherLogListener : public Engine::Serialize::SerializableUnit<LauncherLogListener>,
	public Engine::Util::LogListener
{


public:
	LauncherLogListener();
	~LauncherLogListener();

	void init();

protected:
	// Inherited via LogListener
	virtual void messageLogged(const std::string & message, Engine::Util::MessageType lml, const Engine::Debug::StackTrace<32> &stackTrace, const std::string & logName) override;

private:
	void receiveImpl(const std::string &msg, Engine::Util::MessageType level, const std::string &logName, const std::string &fullTraceback, const std::string &fileName, int lineNr);

private:
	Engine::Serialize::Action<&LauncherLogListener::receiveImpl, Engine::Serialize::ActionPolicy::notification> receiveMessage;
	typedef Engine::SignalSlot::Slot<&decltype(LauncherLogListener::receiveMessage)::operator()> SlotType;
	std::unique_ptr<SlotType> mSlot;
	
};

	}
}
