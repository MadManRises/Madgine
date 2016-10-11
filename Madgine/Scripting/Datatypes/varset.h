#pragma once

#include "valuetype.h"
#include "Serialize/serializable.h"

namespace Engine {
namespace Scripting {


class VarSet : public std::map<std::string, ValueType>, public Serialize::Serializable {
public:

    const ValueType &get(const std::string &name) const;
    bool contains(const std::string &name) const;

protected:
    virtual void save(Serialize::SerializeOutStream &out) const override;
    virtual void load(Serialize::SerializeInStream &in) override;

};



}
}


