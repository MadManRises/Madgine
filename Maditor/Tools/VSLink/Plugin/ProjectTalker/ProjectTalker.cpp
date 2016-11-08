// This is the main DLL file.

#include "ProjectTalker.h"

namespace ProjectTalker {


	ProjectTalker::ProjectTalker(String ^ processId, String ^ name)
	{
		msclr::interop::marshal_context context;
		std::string pId = context.marshal_as<std::string>(processId);
		std::string n = context.marshal_as<std::string>(name);
		ReceiveMessageDelegate ^delegate = gcnew ReceiveMessageDelegate(this, &ProjectTalker::receiveMessageCallback);
		gch = GCHandle::Alloc(delegate);
		IntPtr p = Marshal::GetFunctionPointerForDelegate(delegate);
		MessageReceiver receiver = static_cast<MessageReceiver>(p.ToPointer());
		mNativeTalker = new ProcessTalkerImpl(pId, n, receiver);
	}

	ProjectTalker::~ProjectTalker()
	{
		this->!ProjectTalker();
	}

	bool ProjectTalker::send(int type, String ^ arg1, Int32 arg2, String ^ target) {
		msclr::interop::marshal_context context;
		return mNativeTalker->sendMsg({ type, context.marshal_as<std::string>(arg1), arg2 }, context.marshal_as<std::string>(target));
	}

	void ProjectTalker::run(BoolCarrier ^ b) {
		pin_ptr<bool> ptr = &b->bValue;
		mNativeTalker->run((bool*)ptr);
	}

	void ProjectTalker::receiveMessageCallback(const Message & msg)
	{
		receiveMessage(msg.mType, gcnew String(msg.mArg1.c_str()), msg.mArg2);
	}

}