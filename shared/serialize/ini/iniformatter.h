#pragma once

#include "Meta/serialize/formatter.h"

namespace Engine {
namespace Ini {

struct MADGINE_INI_EXPORT IniFormatter : Serialize::Formatter {
        
		IniFormatter();

        virtual void beginPrimitive(Serialize::SerializeOutStream &, const char *name, uint8_t typeId) override;
                virtual void endPrimitive(Serialize::SerializeOutStream &, const char *name, uint8_t typeId) override;

        virtual void beginPrimitive(Serialize::SerializeInStream &, const char *name, uint8_t typeId) override;
                virtual void endPrimitive(Serialize::SerializeInStream &, const char *name, uint8_t typeId) override;

		virtual std::string lookupFieldName(Serialize::SerializeInStream &) override;
};

}
}