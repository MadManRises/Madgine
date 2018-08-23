#pragma once


namespace Engine
{

	static const char *fix(const char* s)
	{
		const char* f = strrchr(s, ':');
		return f ? f + 1 : s;
	}

	template <class T>
	extern const char *ClassName();

#define RegisterClass(T) template<> const char * Engine::ClassName<T>(){return Engine::fix(#T); } \
	template const char * Engine::ClassName<T>();

}