#pragma once

#include "valuetype.h"

namespace Engine {
namespace Serialize {

	typedef std::ifstream::pos_type pos_type;

class Stream {
public:
	Stream(SerializeManager &mgr);
	virtual ~Stream() = default;

	SerializeManager &manager();

	UI::Process &process();

	ParticipantId id();
	void setId(ParticipantId id);

protected:
	SerializeManager &mManager;
	ParticipantId mId;
};

class MADGINE_EXPORT SerializeInStream : public Stream {
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

	SerializeInStream &operator >> (Ogre::Quaternion &q);


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
		if (v.isNull()) return false;
		val = v.as<T>();
		return true;
	}

	bool loopRead() {
		return ValueType().peek(*this);
	}

    explicit operator bool();


private:
    std::istream &mIfs;
};

class MADGINE_EXPORT SerializeOutStream : public Stream{
public:
    SerializeOutStream(std::ostream &ofs, SerializeManager &mgr);

	SerializeOutStream &operator<<(const ValueType &v);
	SerializeOutStream &operator << (const Ogre::Quaternion &q);

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

