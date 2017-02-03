#pragma once

#include "valuetype.h"

namespace Engine {
namespace Serialize {

	typedef std::istream::pos_type pos_type;

class INTERFACES_EXPORT Stream {
public:
	Stream(SerializeManager &mgr);
	virtual ~Stream() = default;

	SerializeManager &manager();

	Util::Process &process();

	ParticipantId id();
	void setId(ParticipantId id);

protected:
	SerializeManager &mManager;
	ParticipantId mId;
};

class INTERFACES_EXPORT SerializeInStream : public virtual Stream {
public:
    SerializeInStream(std::istream &ifs, SerializeManager &mgr);

	template <class T, typename V = decltype(std::declval<ValueType>().as<T>())>
	SerializeInStream &operator >> (T &v) {
		ValueType temp;
		if ((*this) >> temp)
			v = temp.as<T>();
		return *this;
	}

    SerializeInStream &operator >> (ValueType &result);


    template <class T>
    void read(T &t){
        read(&t, sizeof(T));
    }

    void read(void *buffer, size_t size);

    pos_type tell();
    void seek(pos_type p);

	template <class T>
	bool loopRead(T &val) {
		ValueType v;
		(*this) >> v;
		if (v.isEOL()) return false;
		val = v.as<T>();
		return true;
	}

	bool loopRead();

    explicit operator bool();


private:
    std::istream &mIfs;
};

class INTERFACES_EXPORT SerializeOutStream : public virtual Stream{
public:
    SerializeOutStream(std::ostream &ofs, SerializeManager &mgr);

	SerializeOutStream &operator<<(const ValueType &v);

    virtual void writeData(const void *buffer, size_t size);
    pos_type tell();
    void seek(pos_type p);


	template <class T>
	void write(const T &t) {
		writeData(&t, sizeof(T));
	}

private:
    std::ostream &mOfs;

	
};


}
} // namespace Scripting

