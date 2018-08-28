#pragma once


namespace Engine
{

	template <class Base, class... _Ty>
	using Collector_F = std::unique_ptr<Base>(*)(_Ty...);

#define COLLECTOR_NAME(Name, Collector) \
template<> inline const char *Collector::name(){\
	return "pluginComponents" #Name;\
}
	
#ifdef PLUGIN_BUILD

#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) \
	extern "C" DLL_EXPORT inline const std::vector<Collector::F> *pluginComponents ## Name(size_t baseIndex){ \
		Collector::setBaseIndex(baseIndex); \
		return &Collector::sComponents(); \
	} \
\
	COLLECTOR_NAME(Name, Collector)

#else

#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) COLLECTOR_NAME(Name, Collector)

#endif


	class IndexHolder
	{
	public:
		virtual size_t index() = 0;
	};

	template <class _Base, template <class...> class Container = std::vector, class... _Ty>
	class UniqueComponentCollectorInstance;


	template <class _Base, class... _Ty>
	class UniqueComponentCollector
	{
	public:
		typedef _Base Base;
		typedef Collector_F<Base, _Ty...> F;

		UniqueComponentCollector() = default;
		UniqueComponentCollector(const UniqueComponentCollector&) = delete;
		void operator=(const UniqueComponentCollector&) = delete;


#ifndef PLUGIN_BUILD
		DLL_EXPORT 
#endif
		static std::vector<Collector_F<Base, _Ty...>>& sComponents();


		static void setBaseIndex(size_t index)
		{
			sBaseIndex = index;
		}

	protected:
		template <class T>
		static size_t registerComponent()
		{
			sComponents().emplace_back([](_Ty ... args)
			{
				return std::unique_ptr<Base>(std::make_unique<T>(std::forward<_Ty>(args)...));
			});
			return sComponents().size() - 1;
		}

		static void unregisterComponent(size_t i)
		{
			*std::next(sComponents().begin(), i) = F();
		}

		static inline size_t sBaseIndex = 0;

		static size_t baseIndex()
		{
			return sBaseIndex;
		}


		static const char *name();

	public:
		template <class T>
		class ComponentRegistrator : public IndexHolder
		{
		public:
			ComponentRegistrator()
			{
				mIndex = registerComponent<T>();
			}

			~ComponentRegistrator()
			{
				unregisterComponent(mIndex);
				mIndex = -1;
			}

			size_t index() override
			{
				return mIndex + baseIndex();
			}

		private:
			size_t mIndex = -1;

		};

	};



	template <class _Base, class ... _Ty>
	std::vector<Collector_F<typename UniqueComponentCollector<_Base, _Ty...>::Base, _Ty...>>& UniqueComponentCollector<_Base, _Ty...>::sComponents()
	{
		static std::vector<Collector_F<Base, _Ty...>> dummy;
		return dummy;
	}




}
