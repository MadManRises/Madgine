#include "madginelib.h"

#include "translationunit.h"

#include "messages.h"


namespace Engine {
namespace Database {

const std::string TranslationUnit::sExtension = ".l";


std::string tr(const std::string &key)
{
	static TranslationUnit *unit = &TranslationUnit::getSingleton();
    return unit->getMessage(key);
}

TranslationUnit::TranslationUnit() :
    mRsgm(&Ogre::ResourceGroupManager::getSingleton()),
    mCurrentLanguage(0)
{

    mLoadOrder = 100.0f;
    mScriptPatterns.push_back(std::string("*") + sExtension);
    mRsgm->_registerScriptLoader(this);
}

TranslationUnit::~TranslationUnit()
{
    mRsgm->_unregisterScriptLoader(this);
}

std::string TranslationUnit::getMessage(const std::string &key) const
{
    if (!mCurrentLanguage) return key.c_str();
    auto it = mCurrentLanguage->find(key);
    if (it == mCurrentLanguage->end()) {
        return Messages::unknownKey(key);
    } else {
        return it->second;
    }
}

std::list<std::string> TranslationUnit::availableLanguages()
{
    std::list<std::string> result;
    for (auto &p : mLanguages) {
        result.push_back(p.first);
    }
    return result;
}

std::string TranslationUnit::currentLanguageName()
{
    return mCurrentLanguageName;
}

const std::string &TranslationUnit::resolveLanguageName(const std::string &name)
{
    for (auto &p : *mCurrentLanguage) {
        if (p.second == name)
            return p.first;
    }
    throw 0;
}

void TranslationUnit::setCurrentLanguage(const std::string &name)
{
    auto it = mLanguages.find(name);
    if (it == mLanguages.end()) {
        mCurrentLanguage = 0;
        mCurrentLanguageName.clear();
    } else {
        mCurrentLanguageName = name;
        mCurrentLanguage = &it->second;
    }
}

const Ogre::StringVector &TranslationUnit::getScriptPatterns() const
{
    return mScriptPatterns;
}

float TranslationUnit::getLoadingOrder() const
{
    return mLoadOrder;
}

void TranslationUnit::parseScript(Ogre::DataStreamPtr &stream,
                                  const Ogre::String &)
{

    std::string name = stream->getName();
    std::string baseName = name.substr(0, name.size()-sExtension.size());
    assert(mLanguages.find(baseName) == mLanguages.end());
    std::map<std::string, std::string> &language = mLanguages[baseName];

    Ogre::DataStream &ifs = *stream;

    std::string line = ifs.getLine();

    while (!line.empty()) {
        const char * const pattern = " \t";
        size_t i = line.find_first_of(pattern);
        size_t j = line.find_first_not_of(pattern, i);
        std::string key = line.substr(0, i);
        std::string value = line.substr(j);

        language[key] = value;

        line = ifs.getLine();
    }

    if (!mCurrentLanguage) {
        setCurrentLanguage(baseName);
    }
}


} // namespace Database
} // namespace Game
