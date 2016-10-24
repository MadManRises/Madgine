#pragma once

namespace Engine{
namespace Scene{

class TextureListener{
public:
    virtual void onTextureChanged(Ogre::TexturePtr &tex) = 0;
};

}
}


