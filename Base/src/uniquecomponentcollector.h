#pragma once

#include "Serialize\serializable.h"
#include "Serialize\Container\unithelper.h"

namespace Engine{


template <class Base, class Store>
class UniqueComponentCollector : Store {

public:
	UniqueComponentCollector(const UniqueComponentCollector &) = delete;
	void operator=(const UniqueComponentCollector &) = delete;

    UniqueComponentCollector(){
		for (auto f : this->sComponents()) {
			mComponents.emplace_back(f());
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
		for (std::function<std::unique_ptr<Base>()> &f : Store::sComponents()) {
			result.push_back(&f);
		}
		return result;
	}

	typename std::list<std::unique_ptr<Base>>::const_iterator postCreate(void *hash) {
		auto fIt = std::find_if(this->sComponents().begin(), this->sComponents().end(), [=](const std::function<std::unique_ptr<Base>()> &f) {return &f == hash; });
		return mComponents.insert(mComponents.end(), (*fIt)());
	}

protected:
    template <class T, class _Base, template <class> class Collector>
    friend class UniqueComponent;

    template <class T>
    static typename std::list<std::function<std::unique_ptr<Base>()>>::const_iterator registerComponent(){
		Store::sComponents().emplace_back([]() {return std::unique_ptr<Base>(new T); });
		auto it = Store::sComponents().end();
		--it;
		return it;
    }

	static void unregisterComponent(const typename std::list<std::function<std::unique_ptr<Base>()>>::const_iterator &it) {
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
		typename std::list<std::function<std::unique_ptr<Base>()>>::const_iterator mIterator;
	};

private:
    std::list<std::unique_ptr<Base>> mComponents;
};


template <class Base>
class MADGINE_BASE_EXPORT BaseCreatorStore {
protected:
	static std::list<std::function<std::unique_ptr<Base>()>> &sComponents() {
		static std::list<std::function<std::unique_ptr<Base>()>> dummy;
		return dummy;
	}

};

template <class Base>
class MADGINE_BASE_EXPORT BaseUniqueComponentCollector : public UniqueComponentCollector<Base, BaseCreatorStore<Base>>, public Singleton<BaseUniqueComponentCollector<Base>> {
public:
};


}
