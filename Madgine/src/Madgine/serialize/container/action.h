#pragma once

#include "../observable.h"
#include "../serializehelper.h"
#include "../streams/bufferedstream.h"

namespace Engine
{
	namespace Serialize
	{
		namespace __actionpolicy__impl__{
			template <bool executeOnMasterOnly, bool callByMasterOnly>
			struct _ActionPolicy
			{
				static constexpr bool sExecuteOnMasterOnly = executeOnMasterOnly;
				static constexpr bool sCallByMasterOnly = callByMasterOnly;
			};


			template <auto f, class Config, class T, class R, class... _Ty>
			class ActionImpl : public Observable
			{
			public:

				ActionImpl() :
					mParent(dynamic_cast<T*>(parent()))
				{
					if (!mParent)
						throw 0;
				}

				void operator()(_Ty ... args, const std::set<ParticipantId>& targets = {})
				{
					tryCall(id(), targets, args...);
				}

				void readAction(SerializeInStream& in) override
				{
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
					in >> args;
					TupleUnpacker<const std::set<ParticipantId> &>::call(this, &ActionImpl::call, {}, std::move(args));
				}

				void readRequest(BufferedInOutStream& in) override
				{
					if (!Config::sCallByMasterOnly)
					{
						std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
						in >> args;
						TupleUnpacker<ParticipantId, const std::set<ParticipantId> &>::call(this, &ActionImpl::tryCall, in.id(), {}, args);
					}
				}

				void setVerify(std::function<bool(ParticipantId, _Ty ...)> verify)
				{
					mVerify = verify;
				}


			protected:
				bool verify(ParticipantId id, _Ty ... args)
				{
					return !mVerify || mVerify(id, args...);
				}

			private:
				void call(const std::set<ParticipantId>& targets, _Ty ... args)
				{
					if (!Config::sExecuteOnMasterOnly)
					{
						for (BufferedOutStream* out : getMasterActionMessageTargets(targets))
						{
							*out << std::forward_as_tuple(args...);
							out->endMessage();
						}
					}
					else
					{
						assert(targets.empty());
					}
					(mParent ->* f)(args...);
				}

				void tryCall(ParticipantId id, const std::set<ParticipantId>& targets, _Ty ... args)
				{
					if (verify(id, args...))
					{
						if (isMaster())
						{
							call(targets, args...);
						}
						else
						{
							if (!Config::sCallByMasterOnly && targets.empty())
							{
								BufferedOutStream* out = getSlaveActionMessageTarget();
								(*out << ... << args);
								out->endMessage();
							}
							else
							{
								throw 0;
							}
						}
					}
				}

			private:
				std::function<bool(ParticipantId, _Ty ...)> mVerify;
				T* mParent;
			};
		}
		
		struct ActionPolicy
		{
			//using masterOnly = _ActionPolicy<true, true>;
			using request = __actionpolicy__impl__::_ActionPolicy<true, false>;
			using broadcast = __actionpolicy__impl__::_ActionPolicy<false, false>;
			using notification = __actionpolicy__impl__::_ActionPolicy<false, true>;
		};

		/*template <typename F, F f, class C>
		using Action = typename MemberFunctionCapture<__actionpolicy__impl__::ActionImpl, F, f, C>::type;*/

		template <auto f, class C>
		class Action : public MemberFunctionCapture<__actionpolicy__impl__::ActionImpl, f, C>::type{};

	}
}
