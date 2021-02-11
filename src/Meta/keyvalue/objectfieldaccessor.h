#pragma once

namespace Engine
{
struct META_EXPORT ObjectFieldAccessor
		{
		public:
			ObjectFieldAccessor(const std::shared_ptr<ObjectInstance> &instance, const std::string &name);

			void operator=(const ValueType &v);

			operator ValueType();

		private:
			std::shared_ptr<ObjectInstance> mInstance;
			std::string mName;
		};

}
