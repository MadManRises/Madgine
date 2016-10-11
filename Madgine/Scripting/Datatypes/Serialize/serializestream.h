#pragma once

#include "Scripting/Datatypes/valuetype.h"

namespace Engine {
namespace Scripting {
namespace Serialize {

typedef std::ifstream::pos_type pos_type;

class MADGINE_EXPORT SerializeInStream {
public:
    SerializeInStream(std::istream &ifs);
	SerializeInStream(std::istream &ifs, UI::Process &process);

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

    explicit operator bool();

	UI::Process *process();

private:
    std::istream &mIfs;
	UI::Process *mProcess;
};

class MADGINE_EXPORT SerializeOutStream {
public:
    SerializeOutStream(std::ostream &ofs);

    SerializeOutStream &operator << (const ValueType &v);

	SerializeOutStream &operator << (const Ogre::Quaternion &q);

    template <class T>
    void write(const T &t){
        write(&t, sizeof(T));
    }

    void write(const void *buffer, size_t size);
    pos_type tell();
    void seek(pos_type p);

private:
    std::ostream &mOfs;
};

}
}
} // namespace Scripting

