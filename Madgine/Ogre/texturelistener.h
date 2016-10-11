#pragma once

namespace Engine{
namespace OGRE{

class TextureListener{
public:
    virtual void onTextureChanged(Ogre::TexturePtr &tex) = 0;
};

}
}


