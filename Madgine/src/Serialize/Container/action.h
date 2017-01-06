#pragma once

#include "../observable.h"
#include "templates.h"
#include "Serialize\Streams\bufferedstream.h"

namespace Engine {
	namespace Serialize {

		template <class... _Ty>
		class Action : public Observable{
		public:
			template <class T>
			Action(T *parent, void (T::*callback)(_Ty...)) :
				Observable(parent),
				mImpl([=](_Ty... args) {(parent->*callback)(std::forward<_Ty>(args)...); })
			{

			}

			void operator()(_Ty&&... args) {
				if (isMaster()) {
					call(std::forward<_Ty>(args)...);
				}
				else {
					BufferedOutStream *out = getSlaveActionMessageTarget();
					TupleSerializer::writeTuple(std::forward_as_tuple(std::forward<_Ty>(args)...), *out);
					out->endMessage();
				}
			}

			virtual void readAction(BufferedInOutStream & in) override
			{
				std::tuple<_Ty...> args;
				TupleSerializer::readTuple(args, in);
				TupleUnpacker<>::call(this, &Action::call, std::move(args));
			}

			virtual void readRequest(BufferedInOutStream & in) override
			{
				std::tuple<_Ty...> args;
				TupleSerializer::readTuple(args, in);
				TupleUnpacker<>::call(this, &Action::operator(), std::move(args));
			}

		private:
			void call(_Ty&&... args) {
				mImpl(std::forward<_Ty>(args)...);
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					TupleSerializer::writeTuple(std::forward_as_tuple(std::forward<_Ty>(args)...), *out);
					out->endMessage();
				}
			}

		private:
			std::function<void(_Ty...)> mImpl;



		};

	}
}