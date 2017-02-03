
#include "interfaceslib.h"
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

	TextResource::TextResource(const std::string &name, const std::string &origin) :
		mName(name),
		mLineNr(1),
		mOrigin(origin)
	{
	}

TextResource::~TextResource()
{
}

int TextResource::lineNr()
{
	return mLineNr;
}

void TextResource::setLineNr(int line)
{
	mLineNr = line;
}

const std::string & TextResource::getOrigin() const
{
	return mOrigin;
}

const std::string & TextResource::getName() const
{
	return mName;
}

}
}
}
