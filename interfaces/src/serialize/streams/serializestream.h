#pragma once

#include "valuetype.h"
#include "../serializemanager.h"
#include "../serializeexception.h"

namespace Engine {
namespace Serialize {

	enum class ExtendedValueType : unsigned char {
		UIntValue = static_cast<unsigned char>(ValueType::Type::MAX_TYPE_VALUE),
		SerializableUnitValue
	};

	typedef std::istream::pos_type pos_type;

class INTERFACES_EXPORT Stream {
public:
	Stream(SerializeManager &mgr, ParticipantId id);
	virtual ~Stream() = default;

	SerializeManager &manager();
	bool isMaster();

	Engine::Util::Process &process();

	ParticipantId id();
	void setId(ParticipantId id);

protected:
	SerializeManager &mManager;
	ParticipantId mId;
};

class INTERFACES_EXPORT SerializeInStream : public virtual Stream {
public:
    SerializeInStream(std::istream &ifs, SerializeManager &mgr, ParticipantId id = 0);

	template <class T, typename _ = std::enable_if_t<!std::is_same<T, Scripting::ScopeBase*>::value, decltype(std::declval<ValueType>().as<T>())>>
	SerializeInStream &operator >> (T &v) {
		ValueType temp;
		if ((*this) >> temp)
			v = temp.as<T>();
		return *this;
	}

    SerializeInStream &operator >> (ValueType &result);

	template <class T, typename V = std::enable_if_t<std::is_base_of<SerializableUnitBase, T>::value>>
	SerializeInStream &operator >> (T *&p) {
		ExtendedValueType type;
		read(type);
		if (type != ExtendedValueType::SerializableUnitValue)
			throw SerializeException(Exceptions::notValueType("SerializableUnit"));
		size_t ptr;
		read(ptr);
		p = dynamic_cast<T*>(mManager.convertPtr(*this, ptr));
		if (!p)
			throw 0;
		return *this;
	}

	SerializeInStream &operator >> (Serializable &s);
	SerializeInStream &operator >> (size_t &s);

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
    SerializeOutStream(std::ostream &ofs, SerializeManager &mgr, ParticipantId id = 0);

	SerializeOutStream &operator<<(const ValueType &v);
	template <class T, class _ = std::enable_if_t<!std::is_pointer<T>::value || std::is_convertible<T, const char *>::value, decltype(ValueType(std::declval<T>()))>>
	SerializeOutStream &operator<<(const T& t) {
		return (*this) << ValueType(t);
	}

	SerializeOutStream &operator<<(SerializableUnitBase *p);

	SerializeOutStream &operator<<(const Serializable &s);
	SerializeOutStream &operator<<(size_t s);

    void writeData(const void *buffer, size_t size);
    pos_type tell();
    void seek(pos_type p);


	template <class T>
	void write(const T &t) {
		writeData(&t, sizeof(T));
	}

	explicit operator bool();

private:
    std::ostream &mOfs;

	
};


}
} // namespace Scripting

