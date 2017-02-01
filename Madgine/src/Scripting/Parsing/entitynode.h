#pragma once

#include "methodnode.h"
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class EntityNode : public TextResource {
public:
	using TextResource::TextResource;
	EntityNode(const EntityNode &) = delete;

    void addMethod(const MethodNodePtr &method, const std::string &name);
	void removeMethod(const std::string &name);
	const std::map<std::string, MethodNodePtr> &getMethods() const;
    const MethodNodePtr &getMethod(const std::string &name) const;
    bool hasMethod(const std::string &name) const;
	void setPrototype(const std::string &prototype);
	const std::string &getPrototype() const;
	void clear();

protected:
	virtual void unloadImpl() override final;
	virtual void loadImpl() override final;
	virtual size_t calculateSize() const override final;

private:
    std::map<std::string, MethodNodePtr> mMethods;
	std::string mPrototype;
};

typedef Ogre::SharedPtr<EntityNode> EntityNodePtr;

}
}
}


