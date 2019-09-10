#pragma once

#include "../serialize/formatter.h"

namespace Engine {
namespace Ini {

struct MODULES_EXPORT IniFormatter : Serialize::Formatter {
        
		IniFormatter();

        virtual void beginMember(Serialize::SerializeOutStream &, const char *name, bool first) override;
        virtual void endMember(Serialize::SerializeOutStream &, const char *name, bool first) override;

        virtual void beginMember(Serialize::SerializeInStream &, const char *name, bool first) override;
        virtual void endMember(Serialize::SerializeInStream &, const char *name, bool first) override;

		virtual void writeEOL(Serialize::SerializeOutStream &) override;
        virtual bool readEOL(Serialize::SerializeInStream &) override;

		virtual std::string lookupFieldName(Serialize::SerializeInStream &) override;
};

}
}