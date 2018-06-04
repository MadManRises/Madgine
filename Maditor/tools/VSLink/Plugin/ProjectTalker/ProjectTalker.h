// ProjectTalker.h

#pragma once


#include <msclr\marshal_cppstd.h>
#include "../ProcessTalker32/ProcessTalkerImpl.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace ProjectTalker {


	delegate void ReceiveMessageDelegate(const VSMsg &msg);

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

		bool send(Int64 type, String ^arg1, Int64 arg2, String ^target);

		void run(BoolCarrier ^b);

		
		void receiveMessageCallback(const VSMsg &msg);
		virtual void receiveMessage(int64_t type, String ^arg1, Int64 arg2) abstract;

	};
}
