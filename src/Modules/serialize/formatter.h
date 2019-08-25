#pragma once

namespace Engine {
namespace Serialize {

struct Formatter {
    
	Formatter(bool supportNameLookup = false)
        : mSupportNameLookup(supportNameLookup)
    {
    }

	virtual void beginMember(SerializeOutStream &, const char *name, bool first) = 0;
    virtual void endMember(SerializeOutStream &, const char *name, bool first) = 0;

	virtual void beginMember(SerializeInStream &, const char *name, bool first) = 0;
    virtual void endMember(SerializeInStream &, const char *name, bool first) = 0;

	virtual std::string lookupFieldName(SerializeInStream &)
    {
            return {};
    }

	const bool mSupportNameLookup;

};

}
}