#pragma once


namespace Engine{

	struct StandardHeapCreator {
		template <class T, class... _Ty>
		static T *create(_Ty&&... args) {
			return new T(std::forward<_Ty>(args)...);
		}
	};



template <class Base, class Store, class Creator = StandardHeapCreator, class... _Ty>
class UniqueComponentCollector : Store {

public:
	UniqueComponentCollector(const UniqueComponentCollector &) = delete;
	void operator=(const UniqueComponentCollector &) = delete;

    UniqueComponentCollector(_Ty... args){
		for (auto f : this->sComponents()) {
			mComponents.emplace_back(f(std::forward<_Ty>(args)...));
		}
    }

    typename std::list<std::unique_ptr<Base>>::const_iterator begin() const {
        return mComponents.begin();
    }

    typename std::list<std::unique_ptr<Base>>::const_iterator end() const {
        return mComponents.end();
    }

	size_t size() {
		return mComponents.size();
	}

	const std::list<std::unique_ptr<Base>> &data() const {
		return mComponents;
	}

	static std::list<void*> registeredComponentsHashes() {
		std::list<void*> result;
		for (std::function<std::unique_ptr<Base>(_Ty...)> &f : Store::sComponents()) {
			result.push_back(&f);
		}
		return result;
	}

	typename std::list<std::unique_ptr<Base>>::const_iterator postCreate(void *hash, _Ty... args) {
		auto fIt = std::find_if(this->sComponents().begin(), this->sComponents().end(), [=](const std::function<std::unique_ptr<Base>(_Ty...)> &f) {return &f == hash; });
		return mComponents.insert(mComponents.end(), (*fIt)(std::forward<_Ty>(args)...));
	}

protected:
    template <class T, template <class, class, class...> class Collector, class _Base, class _Creator, class...>
    friend class UniqueComponent;

    template <class T>
    static typename std::list<std::function<std::unique_ptr<Base>(_Ty...)>>::const_iterator registerComponent(){
		Store::sComponents().emplace_back([](_Ty... args) {return std::unique_ptr<Base>(Creator::template create<T>(std::forward<_Ty>(args)...)); });
		auto it = Store::sComponents().end();
		--it;
		return it;
    }

	static void unregisterComponent(const typename std::list<std::function<std::unique_ptr<Base>(_Ty...)>>::const_iterator &it) {
		Store::sComponents().erase(it);
	}

public:
	template <class T>
	class ComponentRegistrator {
	public:
		ComponentRegistrator() {
			mIterator = registerComponent<T>();
		}
		~ComponentRegistrator() {
			unregisterComponent(mIterator);
		}
	private:
		typename std::list<std::function<std::unique_ptr<Base>(_Ty...)>>::const_iterator mIterator;
	};

private:
    std::list<std::unique_ptr<Base>> mComponents;
};


template <class Base, class... _Ty>
class MADGINE_BASE_EXPORT BaseCreatorStore {
protected:
	static std::list<std::function<std::unique_ptr<Base>(_Ty...)>> &sComponents() {
		static std::list<std::function<std::unique_ptr<Base>(_Ty...)>> dummy;
		return dummy;
	}

};

template <class Base, class Creator = StandardHeapCreator, class... _Ty>
class MADGINE_BASE_EXPORT BaseUniqueComponentCollector : 
	public UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Creator, _Ty...>, 
	public Singleton<BaseUniqueComponentCollector<Base, Creator, _Ty...>>
{
	using UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Creator, _Ty...>::UniqueComponentCollector;
};


}
