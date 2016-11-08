#pragma once



#include <string>
#include <map>



namespace boost {
	namespace interprocess {

		template < class T, class DifferenceType = int32_t
			, class OffsetType = uint64_t, std::size_t Alignment = 0>
			class offset_ptr;

		template<class VoidPointer>
		class message_queue_t;

		typedef message_queue_t<offset_ptr<void> > message_queue;
	}
}

using namespace boost::interprocess;

struct Message {
	int64_t mType;
	std::string mArg1;
	int32_t mArg2;
};

class ProcessTalker {
public:
	ProcessTalker(const std::string &processId, const std::string &name);
	~ProcessTalker();

	void update();
	void run(bool *b);

	bool sendMsg(const Message &msg, const std::string &target);
	virtual void receiveMessage(const Message &msg) = 0;

private:

	struct InternalMessage {
		int64_t mType;
		char mArg1[256];
		int32_t mArg2;
	} mBuffer;

	std::string mFullName;
	std::string mProcessId;
	message_queue *mReadQueue;

};