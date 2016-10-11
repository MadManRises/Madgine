#pragma once

namespace Ogre{

template <class T>
using unique_ptr = std::unique_ptr<T>;

template <class T, class Base>
constexpr std::function<unique_ptr<Base>()> make_unique(const char *file, int line, const char *func){
    return [=](){
        return unique_ptr<Base>(new (file, line, func) T);
    };
}

template <class T, class Base = T>
struct MakeUniqueHelper{
    MakeUniqueHelper(const char *file, int line, const char *func) :
        mFile(file), mLine(line), mFunc(func) {}

    template <class... Ty>
    unique_ptr<Base> operator()(Ty&&... args){
        return unique_ptr<Base>(new (mFile, mLine, mFunc) T(std::forward<Ty>(args)...));
    }

private:
    const char *const mFile;
    const char *const mFunc;
    int mLine;
};

}

#define OGRE_MAKE_UNIQUE_FUNC(T, Base) Ogre::make_unique<T, Base>(__FILE__, __LINE__, __FUNCTION__)
#define OGRE_MAKE_UNIQUE(T) Ogre::MakeUniqueHelper<T>(__FILE__, __LINE__, __FUNCTION__)
#define OGRE_MAKE_UNIQUE_BASE(T, Base) Ogre::MakeUniqueHelper<T, Base>(__FILE__, __LINE__, __FUNCTION__)



