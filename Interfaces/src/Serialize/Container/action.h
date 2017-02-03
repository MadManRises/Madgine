#pragma once

#include "../observable.h"
#include "templates.h"
#include "Serialize\Streams\bufferedstream.h"

namespace Engine {
	namespace Serialize {

		struct _ActionPolicy {
			bool mExecuteOnMasterOnly;
			bool mCallByMasterOnly;
		};

		struct ActionPolicy{
			static const constexpr _ActionPolicy masterOnly{ true, true };
			static const constexpr _ActionPolicy standard{ true, false };
			static const constexpr _ActionPolicy allowAll{ false, false };
		};

		template <const _ActionPolicy &Config, class... _Ty>
		class Action : public Observable{
		public:
			template <class T>
			Action(T *parent, void (T::*callback)(_Ty...)) :
				Observable(parent),
				mImpl([=](_Ty... args) {(parent->*callback)(args...); })
			{

			}

			void operator()(_Ty... args) {
				if (!verify(args...))
					return;
				if (isMaster()) {
					call(args...);
				}
				else {
					if (!Config.mCallByMasterOnly) {
						BufferedOutStream *out = getSlaveActionMessageTarget();
						TupleSerializer::writeTuple(std::forward_as_tuple(args...), *out);
						out->endMessage();
					}
					else {
						throw 0;
					}
				}
			}

			virtual void readAction(BufferedInOutStream & in) override
			{
				std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
				TupleSerializer::readTuple(args, in);
				TupleUnpacker<>::call(this, &Action::call, std::move(args));
			}

			virtual void readRequest(BufferedInOutStream & in) override
			{
				if (!Config.mCallByMasterOnly) {
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
					TupleSerializer::readTuple(args, in);
					if (!TupleUnpacker<>::call(this, &Action::verify, args))
						return;
					TupleUnpacker<>::call(this, &Action::operator(), args);
				}
			}

			void setVerify(std::function<bool(_Ty...)> verify) {
				mVerify = verify;
			}

		protected:
			bool verify(_Ty... args) {
				return !mVerify || mVerify(args...);
			}

		private:
			void call(_Ty... args) {
				mImpl(std::forward<_Ty>(args)...);
				if (!Config.mExecuteOnMasterOnly) {
					for (BufferedOutStream *out : getMasterActionMessageTargets()) {
						TupleSerializer::writeTuple(std::forward_as_tuple(args...), *out);
						out->endMessage();
					}
				}
			}

		private:
			std::function<void(_Ty...)> mImpl;
			std::function<bool(_Ty...)> mVerify;
		};

	}
}