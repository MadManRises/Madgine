#include "../moduleslib.h"

#include "xmlformatter.h"

#include "../serialize/streams/serializestream.h"

#include "Interfaces/stringutil.h"

namespace Engine {
namespace XML {

	struct xml_ctype : std::ctype<char> {
        static const mask *make_table()
        {
            // make a copy of the "C" locale table
            static std::vector<mask> v(classic_table(), classic_table() + table_size);
            v['"'] |= space; // comma will be classified as whitespace            
            return &v[0];
        }
        xml_ctype(std::size_t refs = 0)
            : ctype(make_table(), false, refs)
        {
        }
    };

    XMLFormatter::XMLFormatter()
        : Formatter(false, true)
    {
    }

    void XMLFormatter::setupStream(std::istream &in)
    {
        in.imbue(std::locale(in.getloc(), new xml_ctype));
    }

	void XMLFormatter::setupStream(std::ostream &out)
    {
        out.imbue(std::locale(out.getloc(), new xml_ctype));
    }
    
	void XMLFormatter::beginExtendedCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        if (!mCurrentExtended) {
            out.writeUnformatted(indent() + "<" + std::string(name));
            mCurrentExtended = true;
        }
    }

    void XMLFormatter::beginCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        if (!mCurrentExtended) {
            out.writeUnformatted(indent() + "<" + std::string(name));
        }
        out.writeUnformatted(">\n");
        mCurrentExtended = false;
        ++mLevel;
    }

    void XMLFormatter::endCompound(Serialize::SerializeOutStream &out, const char *name)
    {
        --mLevel;
        out.writeUnformatted(indent() + "</" + std::string(name) + ">\n");
    }

    void XMLFormatter::beginExtendedCompound(Serialize::SerializeInStream &in, const char *name)
    {
        if (!mCurrentExtended) {
            std::string prefix = in.readN(strlen(name) + 2);
            if (!StringUtil::startsWith(prefix, "<" + std::string(name)))
                throw 0;
            mCurrentExtended = true;
        }
    }

    void XMLFormatter::beginCompound(Serialize::SerializeInStream &in, const char *name)
    {
        if (!mCurrentExtended) {
            std::string prefix = in.readN(strlen(name) + 1);
            if (prefix != "<" + std::string(name))
                throw 0;
        }
        std::string prefix = in.readN(1);
        if (prefix != ">")
            throw 0;
        mCurrentExtended = false;
    }

    void XMLFormatter::endCompound(Serialize::SerializeInStream &in, const char *name)
    {
        std::string prefix = in.readN(strlen(name) + 3);
        if (prefix != "</" + std::string(name) + ">")
            throw 0;
    }

    void XMLFormatter::beginPrimitive(Serialize::SerializeOutStream &out, const char *name, size_t typeId)
    {
        if (mCurrentExtended) {
            out.writeUnformatted(" " + std::string(name) + "=");
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
                out.writeUnformatted("\"");
        } else
            out.writeUnformatted(indent() + "<" + std::string(name) + ">");
    }

    void XMLFormatter::endPrimitive(Serialize::SerializeOutStream &out, const char *name, size_t typeId)
    {
        if (mCurrentExtended) {
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
                out.writeUnformatted("\"");
        } else
            out.writeUnformatted("</" + std::string(name) + ">\n");
    }

    void XMLFormatter::beginPrimitive(Serialize::SerializeInStream &in, const char *name, size_t typeId)
    {
        if (mCurrentExtended) {
            if (name) {
                std::string prefix = in.readN(strlen(name) + 1);
                if (prefix != std::string(name) + "=")
                    throw 0;
            } else {
                if (in.readUntil("=").empty())
                    throw 0;
            }
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
                if (in.readN(1) != "\"")
                    throw 0;
        } else {
            std::string prefix = in.readN(strlen(name) + 2);
            if (prefix != "<" + std::string(name) + ">")
                throw 0;
        }
    }

    void XMLFormatter::endPrimitive(Serialize::SerializeInStream &in, const char *name, size_t typeId)
    {
        if (mCurrentExtended) {
            if (typeId == Serialize::PrimitiveTypeIndex_v<std::string>)
                if (in.readN(1) != "\"")
                    throw 0;
        } else {
            std::string prefix = in.readN(strlen(name) + 3);
            if (prefix != "</" + std::string(name) + ">")
                throw 0;
        }
    }

    std::string XMLFormatter::lookupFieldName(Serialize::SerializeInStream &in)
    {
        std::string name = in.peekUntil("> ");
        if (!name.empty()) {
            if (name[0] != '<')
                throw 0;
            if (name[1] == '/')
                return {};
            return name.substr(1, name.size() - 2);
        }
        return name;
    }

    std::string XMLFormatter::indent()
    {
        return std::string(4 * mLevel, ' ');
    }

}
}
