#pragma once

#include "../observable.h"
#include "generic/templates.h"
#include "Serialize\Streams\bufferedstream.h"

namespace Engine {
	namespace Serialize {

		template <bool executeOnMasterOnly, bool callByMasterOnly>
		struct _ActionPolicy {
			static constexpr bool sExecuteOnMasterOnly = executeOnMasterOnly;
			static constexpr bool sCallByMasterOnly = callByMasterOnly;
		};

		struct ActionPolicy{
			//using masterOnly = _ActionPolicy<true, true>;
			using request = _ActionPolicy<true, false>;
			using broadcast = _ActionPolicy<false, false>;
			using notification = _ActionPolicy<false, true>;
		};

		template <class F, F f, class Config, class T, class R, class... _Ty>
		class ActionImpl : public Observable {
		public:			

			ActionImpl() :
				mParent(dynamic_cast<T*>(parent()))
			{
				if (!mParent)
					throw 0;
			}

			void operator()(_Ty... args, const std::list<ParticipantId> &targets = {}) {
				tryCall(id(), targets, args...);
			}

			virtual void readAction(SerializeInStream & in) override
			{
				std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
				TupleSerializer::readTuple(args, in);
				TupleUnpacker<const std::list<ParticipantId> &>::call(this, &ActionImpl::call, {}, std::move(args));
			}

			virtual void readRequest(BufferedInOutStream & in) override
			{
				if (!Config::sCallByMasterOnly) {
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
					TupleSerializer::readTuple(args, in);
					TupleUnpacker<ParticipantId, const std::list<ParticipantId> &>::call(this, &ActionImpl::tryCall, in.id(), {}, args);
				}
			}

			void setVerify(std::function<bool(ParticipantId, _Ty...)> verify) {
				mVerify = verify;
			}


		protected:
			bool verify(ParticipantId id, _Ty... args) {
				return !mVerify || mVerify(id, args...);
			}

		private:
			void call(const std::list<ParticipantId> &targets, _Ty... args) {
				if (!Config::sExecuteOnMasterOnly) {
					for (BufferedOutStream *out : getMasterActionMessageTargets(targets)) {
						TupleSerializer::writeTuple(std::forward_as_tuple(args...), *out);
						out->endMessage();
					}
				}
				else {
					assert(targets.empty());
				}
				(mParent->*f)(args...);
			}

			void tryCall(ParticipantId id, const std::list<ParticipantId> &targets, _Ty... args) {
				if (verify(id, args...)) {
					if (isMaster()) {
						call(targets, args...);
					}
					else {
						if (!Config::sCallByMasterOnly && targets.empty()) {
							BufferedOutStream *out = getSlaveActionMessageTarget();
							TupleSerializer::writeTuple(std::forward_as_tuple(args...), *out);
							out->endMessage();
						}
						else {
							throw 0;
						}
					}
				}
			}

		private:
			std::function<bool(ParticipantId, _Ty...)> mVerify;
			T *mParent;
		};

		/*template <auto f, class C>
		using Action = MemberFunctionWrapper<ActionImpl, f, C>;*/

		template <typename F, F f, class C>
		using Action = typename MemberFunctionCapture<ActionImpl, F, f, C>::type;

	}
}