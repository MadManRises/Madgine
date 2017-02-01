#pragma once



namespace Engine {
namespace Scripting {
namespace Parsing {

class MADGINE_EXPORT TextResource : public Ogre::Resource {
public:
    TextResource(Ogre::ResourceManager *creator, const Ogre::String &name,
		Ogre::ResourceHandle handle, const Ogre::String &group, bool isManual = false,
		Ogre::ManualResourceLoader *loader = 0);
	TextResource(const TextResource &) = delete;
	virtual ~TextResource();    

	int lineNr();
	void setLineNr(int line);

private:
	int mLineNr;
};

typedef Ogre::SharedPtr<TextResource> TextResourcePtr;

}
}
}


