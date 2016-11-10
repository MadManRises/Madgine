#pragma once

namespace Engine{
namespace Resources{

class TextureListener{
public:
    virtual void onTextureChanged(Ogre::TexturePtr &tex) = 0;
};

}
}


