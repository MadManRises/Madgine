#pragma once

#include "methodnode.h"
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class INTERFACES_EXPORT EntityNode : public TextResource {
public:
	using TextResource::TextResource;
	EntityNode(const EntityNode &) = delete;

    MethodNode &addMethod(const std::string &name, const std::string &origin);
	void removeMethod(const std::string &name);
	const std::map<std::string, MethodNode> &getMethods() const;
    const MethodNode &getMethod(const std::string &name) const;
    bool hasMethod(const std::string &name) const;
	void setPrototype(const std::string &prototype);
	const std::string &getPrototype() const;
	void clear();

private:
    std::map<std::string, MethodNode> mMethods;
	std::string mPrototype;
};

}
}
}


