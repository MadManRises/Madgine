// ProjectTalker.h

#pragma once


#include <msclr\marshal_cppstd.h>
#include "../ProcessTalker32/ProcessTalkerImpl.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace ProjectTalker {


	delegate void ReceiveMessageDelegate(const Message &msg);

	public ref struct BoolCarrier {
	public:
		bool bValue;
	};

	public ref class ProjectTalker
	{
		ProcessTalkerImpl *mNativeTalker;
		GCHandle gch;

	public:
		ProjectTalker(String ^processId, String ^name);
		~ProjectTalker();
		!ProjectTalker() {
			delete mNativeTalker;
			gch.Free();
		}

		bool send(int type, String ^arg1, Int32 arg2, String ^target);

		void run(BoolCarrier ^b);

		
		void receiveMessageCallback(const Message &msg);
		virtual void receiveMessage(int64_t type, String ^arg1, Int32 arg2) abstract;

	};
}
