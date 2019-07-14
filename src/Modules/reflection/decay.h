#pragma once

namespace Engine {

	template <typename T>
	struct decay {
		using type = void;
	};

	template <typename T>
	using decay_t = typename decay<T>::type;

	template <typename T, typename Decay>
    struct decayed {
		using type = typename decayed<Decay, decay_t<Decay>>::type;
    };

	template <typename T>
    struct decayed<T, void> {
		using type = T;
	};

	template <typename T>
	using decayed_t = typename decayed<T, decay_t<T>>::type;

}