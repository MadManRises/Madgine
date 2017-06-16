#pragma once


namespace Engine {
namespace Database {


class OGREMADGINE_EXPORT TranslationKey {
public:
    TranslationKey(const std::string &key);

    operator std::string() const;

private:
    std::string mKey;
};

}
} // namespace Database

