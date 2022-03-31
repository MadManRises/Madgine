#pragma once
		
template <typename T, typename U>
concept nots = !std::same_as<T, U>;

DLL_IMPORT_VARIABLE(const Engine::MetaTable, table, nots<Engine::Void>);
