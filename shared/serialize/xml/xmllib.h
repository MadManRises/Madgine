#pragma once

#include "Meta/metalib.h"

#if defined(Xml_EXPORTS)
#    define MADGINE_XML_EXPORT DLL_EXPORT
#else
#    define MADGINE_XML_EXPORT DLL_IMPORT
#endif