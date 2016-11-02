#pragma once

#include "methodnodeptr.h"
#include "textResource.h"

namespace Engine {
namespace Scripting {
namespace Parsing {

class EntityNode : public TextResource {
public:
    EntityNode(Ogre::ResourceManager *creator, const Ogre::String &name,
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
		Ogre::ManualResourceLoader *loader = 0);
	EntityNode(const EntityNode &) = delete;

    void addMethod(const MethodNodePtr &method, const std::string &name);
	void removeMethod(const std::string &name);
	const std::map<std::string, MethodNodePtr> &getMethods() const;
    const MethodNodePtr &getMethod(const std::string &name) const;
    bool hasMethod(const std::string &name) const;
	void setPrototype(Scope *prototype);
	Scope *getPrototype() const;
	void clear();

protected:
	virtual void unloadImpl() override final;
	virtual void loadImpl() override final;
	virtual size_t calculateSize() const override final;

private:
    std::map<std::string, MethodNodePtr> mMethods;
	Scope *mPrototype;
};



}
}
}


