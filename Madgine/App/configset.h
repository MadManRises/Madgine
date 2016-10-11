#pragma once

#include "Scripting\Datatypes\varset.h"
#include "Database\translationunit.h"

namespace Engine {
namespace App {

class MADGINE_EXPORT ConfigSet : public Ogre::Singleton<ConfigSet>, public Ogre::GeneralAllocatedObject {
public:
    ConfigSet(Ogre::Root *root, const std::string &configFileName);

    bool getFullscreen();
    bool getVSync();
    int getResolutionWidth();
    int getResolutionHeight();
    int getColorDepth();
    std::string getLanguage();
    std::string getRenderSystem();

    std::string getResolutionString();
    std::string getResolutionAndColorDepthString();

    void setFullscreen(bool b);
    void setResolutionWidth(int i);
    void setResolutionHeight(int i);
    void setResolutionString(const std::string &s);
    void setColorDepth(int i);
    void setLanguage(const std::string &language);
    void applyLanguage();

    void updateWindow();

    void save();
    void load();

    Ogre::StringVector &getPossibleVideoModes();
    Ogre::StringVector &getPossibleColorDepths();


private:

    static const bool defaultFullScreen;
    static const bool defaultVSync;
    static const int defaultResolutionWidth;
    static const int defaultResolutionHeight;
    static const int defaultColorDepth;
    static const std::string defaultRenderSystem;
    static const std::string defaultLanguage;

    static const std::string fullScreenProperty;
    static const std::string vSyncProperty;
    static const std::string heightProperty;
    static const std::string widthProperty;
    static const std::string colorDepthProperty;
    static const std::string renderSystemProperty;
    static const std::string videoModeProperty;
    static const std::string languageProperty;

    static const std::string yesString;
    static const std::string noString;

private:
    Scripting::VarSet mSettings;
    Ogre::RenderSystem *mRenderSystem;


    Database::TranslationUnit mTranslation;

    std::string mConfigFileName;


};

}
}
