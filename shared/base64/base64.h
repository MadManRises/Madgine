

#if defined(Base64_EXPORTS)
#    define BASE64_EXPORT DLL_EXPORT
#else
#    define BASE64_EXPORT DLL_IMPORT
#endif

namespace Engine {
namespace Base64 {

	BASE64_EXPORT std::string encode(const ByteBuffer &data);
    BASE64_EXPORT bool decode(ByteBuffer &b, std::string_view string);

}
}