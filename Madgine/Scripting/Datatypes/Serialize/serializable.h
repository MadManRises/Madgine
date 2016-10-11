#pragma once

namespace Engine {
namespace Scripting {
namespace Serialize {


MADGINE_EXPORT SerializeInStream operator >> (SerializeInStream &in, Serializable &s);
MADGINE_EXPORT SerializeOutStream operator << (SerializeOutStream &out, const Serializable &s);

class Serializable
{
protected:
    virtual void save(Serialize::SerializeOutStream &out) const = 0;
    virtual void load(Serialize::SerializeInStream &in) = 0;

private:
    friend MADGINE_EXPORT SerializeInStream operator >> (SerializeInStream &in, Serializable &s);
    friend MADGINE_EXPORT SerializeOutStream operator << (SerializeOutStream &out, const Serializable &s);
};


} // namespace Serialize
} // namespace Scripting
} // namespace Core

