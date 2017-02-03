#pragma once


namespace Engine{

template <class Base>
class MADGINE_EXPORT OgreUniqueComponentCollector : public Singleton<OgreUniqueComponentCollector<Base>>{

public:
	OgreUniqueComponentCollector(const OgreUniqueComponentCollector &) = delete;
	void operator=(const OgreUniqueComponentCollector &) = delete;

    OgreUniqueComponentCollector(){
        for (auto f : sComponents())
            mComponents.emplace_back(f());
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


private:
    template <class T, class _Base>
    friend class OgreUniqueComponent;

    template <class T>
    static typename std::list<std::function<std::unique_ptr<Base>()>>::const_iterator registerComponent(){
		auto f = OGRE_MAKE_UNIQUE_FUNC(T, Base);
		OgreUniqueComponentCollector *self = OgreUniqueComponentCollector<Base>::getSingletonPtr();
		if (self) {
			Ogre::unique_ptr<Base> component = f();
			self->mComponents.emplace_back(std::move(component));
		}
        sComponents().emplace_back(f);
		auto it = sComponents().end();
		--it;
		return it;
    }

	static void unregisterComponent(const typename std::list<std::function<std::unique_ptr<Base>()>>::const_iterator &it) {
		typename std::list<std::function<std::unique_ptr<Base>()>>::const_iterator begin = sComponents().begin();
		size_t i = std::distance(begin, it);
		OgreUniqueComponentCollector *self = OgreUniqueComponentCollector<Base>::getSingletonPtr();
		if (self) {
			auto it2 = self->mComponents.begin();
			std::advance(it2, i);
			self->mComponents.erase(it2);
		}
		sComponents().erase(it);
	}

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

    static std::list<std::function<std::unique_ptr<Base>()>> &sComponents(){
        static std::list<std::function<std::unique_ptr<Base>()>> dummy;
        return dummy;
    }

    std::list<std::unique_ptr<Base>> mComponents;
};

}
