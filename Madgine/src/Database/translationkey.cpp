#include "libinclude.h"
#include "translationkey.h"
#include "translationunit.h"

namespace Engine {
namespace Database {

TranslationKey::TranslationKey(const std::string &key) :
    mKey(key)
{

}

TranslationKey::operator std::string() const
{
    return tr(mKey);
}


}
} // namespace Database

