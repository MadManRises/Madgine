#pragma once

#include "../observable.h"
#include "../streams/bufferedstream.h"
#include "serializablecontainer.h"
#include "sortedcontainer.h"
#include "unsortedcontainer.h"

namespace Engine
{
	namespace Serialize
	{
		enum Operations
		{
			INSERT_ITEM = 0x1,
			REMOVE_ITEM = 0x2,
			RESET = 0x3,


			MASK = 0x0F,
			ACCEPT = 0x10,
			REJECT = 0x20,

			BEFORE = 0x0,
			AFTER = 0x10
		};

		struct _ContainerPolicy
		{
			enum RequestMode
			{
				ALL_REQUESTS,
				NO_REQUESTS
			} mRequestMode;
		};

		struct ContainerPolicy
		{
			static const constexpr _ContainerPolicy allowAll{_ContainerPolicy::ALL_REQUESTS};
			static const constexpr _ContainerPolicy masterOnly{_ContainerPolicy::NO_REQUESTS};
		};

		template <class traits, class Creator>
		using ContainerSelector = std::conditional_t<
			traits::sorted,
			SortedContainer<traits, Creator>,
			UnsortedContainer<traits, Creator>
		>;

		template <class traits, class Creator, const _ContainerPolicy &Config>
		class ObservableContainer :
			public ContainerSelector<traits, Creator>, public Observable
		{
		public:
			typedef size_t TransactionId;

			typedef ContainerSelector<traits, Creator> Base;

			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;
			typedef typename traits::type type;
			typedef typename traits::value_type value_type;


			struct Transaction
			{
				template <class T>
				Transaction(TransactionId id, const T& callback) :
					mId(id), mCallback(callback)
				{
				}

				TransactionId mId;
				SignalSlot::Connection<const iterator &, bool> mCallback;
			};


			ObservableContainer() :
				mTransactionCounter(0)
			{
			}

			ObservableContainer(const ObservableContainer& other) :
				Base(other),
				mTransactionCounter(0)
			{
			}

			ObservableContainer(ObservableContainer&& other) = default;

			virtual ~ObservableContainer()
			{
			}

			template <class T>
			void operator=(T&& arg)
			{
				if (isMaster())
				{
					bool wasActive = beforeReset(this->end());
					Base::operator=(std::forward<T>(arg));
					afterReset(wasActive, this->end());
				}
				else
				{
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS)
					{
						Base temp(std::forward<T>(arg));

						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << TransactionId(0);
						*out << RESET;
						temp.writeState(*out);
						out->endMessage();
					}
					else
					{
						throw 0;
					}
				}
			}


			void clear()
			{
				bool wasActive = beforeReset(this->end());
				this->mData.clear();
				this->mLocallyActiveIterator = this->mData.begin();
				afterReset(wasActive, this->end());
			}


			template <class... _Ty>
			std::pair<iterator, bool> emplace(const iterator& where, _Ty&&... args)
			{
				std::pair<iterator, bool> it = std::make_pair(this->end(), false);

				if (isMaster())
				{
					it = Base::emplace_intern(where, std::forward<_Ty>(args)...);
					if (it.second)
						onInsert(it.first);
				}
				else
				{
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS)
					{
						type temp(std::forward<_Ty>(args)...);
						this->postConstruct(temp);

						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << TransactionId(0);
						*out << INSERT_ITEM;
						this->write_item(*out, where, temp);
						out->endMessage();
					}
					else
					{
						throw 0;
					}
				}
				return it;
			}

			template <class T, class... _Ty>
			std::pair<iterator, bool> emplace_init(T&& init, const iterator& where, _Ty&&... args)
			{
				std::pair<iterator, bool> it = std::make_pair(this->end(), false);
				if (isMaster())
				{
					it = Base::emplace_intern(where, std::forward<_Ty>(args)...);
					if (it.second)
					{
						init(*it.first);
						onInsert(it.first);
					}
				}
				else
				{
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS)
					{
						type temp(std::forward<_Ty>(args)...);
						this->postConstruct(temp);

						init(temp);

						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << TransactionId(0);
						*out << INSERT_ITEM;
						this->write_item(*out, where, temp);
						out->endMessage();
					}
					else
					{
						throw 0;
					}
				}
				return it;
			}

			iterator erase(const iterator& where)
			{
				iterator it = this->end();

				if (isMaster())
				{
					bool b = beforeRemove(where);
					it = Base::erase_intern(where);
					afterRemove(b);
				}
				else
				{
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS)
					{
						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << TransactionId(0);
						*out << REMOVE_ITEM;
						this->write_iterator(*out, where);
						out->endMessage();
					}
					else
					{
						throw 0;
					}
				}
				return it;
			}


			std::pair<iterator, bool> performOperation(Operations op, SerializeInStream& in, ParticipantId partId,
			                                           TransactionId id)
			{
				std::pair<iterator, bool> it = std::make_pair(this->end(), false);
				bool b;
				switch (op)
				{
				case INSERT_ITEM:
					it = this->read_item(in);
					if (it.second)
						onInsert(it.first, partId, id);
					break;
				case REMOVE_ITEM:
					it.first = this->read_iterator(in);
					b = beforeRemove(it.first, partId, id);
					it.first = Base::erase_intern(it.first);
					afterRemove(b);
					it.second = true;
					break;
				case RESET:
					readStateImpl(in, partId, id);
					it.second = true;
					break;
				default:
					throw 0;
				}
				return it;
			}

			virtual void readState(SerializeInStream& in) override
			{
				readStateImpl(in);
			}

			void readStateImpl(SerializeInStream& in, ParticipantId answerTarget = 0, TransactionId answerId = 0)
			{
				bool wasActive = beforeReset(this->end());
				this->mData.clear();
				this->mLocallyActiveIterator = this->mData.begin();
				while (in.loopRead())
				{
					this->read_item_where_intern(this->end(), in);
				}
				afterReset(wasActive, this->end());
			}

			// Inherited via Observable
			void readRequest(BufferedInOutStream& inout) override
			{
				bool accepted = Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS; //Check TODO

				TransactionId id;
				inout >> id;

				Operations op;
				inout >> reinterpret_cast<int&>(op);


				if (!accepted)
				{
					if (id)
					{
						beginActionResponseMessage(&inout);
						inout << id;
						inout << (op | REJECT);
						inout.endMessage();
					}
				}
				else
				{
					if (isMaster())
					{
						performOperation(op, inout, inout.id(), id);
					}
					else
					{
						TransactionId newId = 0;
						if (id)
						{
							newId = ++mTransactionCounter;
						}
						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << newId;
						*out << op;
						*out << inout;
						out->endMessage();

						if (id)
						{
							mPassTransactions.emplace_back(newId, std::make_pair(inout.id(), id));
						}
					}
				}
			}

			void readAction(SerializeInStream& inout) override
			{
				TransactionId id;
				inout >> id;

				Operations op;
				inout >> op;

				bool accepted = (id == 0) || ((op & (~MASK)) == ACCEPT);

				iterator it = this->end();

				std::pair<ParticipantId, TransactionId> sender = {0, 0};
				if (id)
				{
					if (!mPassTransactions.empty() && mPassTransactions.front().first == id)
					{
						sender = mPassTransactions.front().second;
						mPassTransactions.pop_front();
					}
				}

				if (accepted)
				{
					it = performOperation(Operations(op & MASK), inout, sender.first, sender.second).first;
				}

				if (id)
				{
					if (!mTransactions.empty() && mTransactions.front().mId == id)
					{
						mTransactions.front().mCallback(it, accepted);
						mTransactions.pop_front();
					}
				}
			}

			template <class Ty>
			void connectCallback(Ty&& slot)
			{
				mSignal.connect(std::forward<Ty>(slot));
			}

		protected:
			virtual void notifySetActive(bool active)
			{
				if (!active)
				{
					while (this->mLocallyActiveIterator != this->begin())
					{
						--this->mLocallyActiveIterator;
						mSignal.emit(this->mLocallyActiveIterator, BEFORE | REMOVE_ITEM);
						mSignal.emit(this->end(), AFTER | REMOVE_ITEM);
						this->notifySetItemActive(*this->mLocallyActiveIterator, active);
					}
				}
				Serializable::notifySetActive(active);
				if (active)
				{
					while (this->mLocallyActiveIterator != this->end())
					{
						auto it = this->mLocallyActiveIterator;
						++this->mLocallyActiveIterator;
						this->notifySetItemActive(*it, active);
						mSignal.emit(it, INSERT_ITEM);
					}
				}
			}

		private:
			void onInsert(const iterator& it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
			{
				if (this->isActive())
				{
					for (BufferedOutStream* out : getMasterActionMessageTargets())
					{
						if (answerTarget == out->id())
						{
							*out << answerId;
						}
						else
						{
							*out << TransactionId(0);
						}
						*out << INSERT_ITEM;
						this->write_item(*out, it);
						out->endMessage();
					}
					this->setItemActiveFlag(*it, true);
				}
				if (isItemLocallyActive(it))
				{
					this->notifySetItemActive(*it, true);
					mSignal.emit(it, INSERT_ITEM);
				}
			}

			bool beforeRemove(const iterator& it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
			{
				if (this->isActive())
				{
					for (BufferedOutStream* out : getMasterActionMessageTargets())
					{
						if (answerTarget == out->id())
						{
							*out << answerId;
						}
						else
						{
							*out << TransactionId(0);
						}
						*out << REMOVE_ITEM;
						this->write_iterator(*out, it);
						out->endMessage();
					}
					this->setItemActiveFlag(*it, false);
				}
				if (this->isItemLocallyActive(it))
				{
					mSignal.emit(it, BEFORE | REMOVE_ITEM);
					this->notifySetItemActive(*it, false);
					return true;
				}
				return false;
			}

			void afterRemove(bool b)
			{
				if (b)
				{
					mSignal.emit(this->end(), AFTER | REMOVE_ITEM);
				}
			}

			bool beforeReset(const iterator& it)
			{
				if (this->isLocallyActive())
				{
					mSignal.emit(it, BEFORE | RESET);
				}
				return this->deactivate();
			}

			void afterReset(bool wasActive, const iterator& it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
			{
				if (this->isActive())
				{
					for (BufferedOutStream* out : getMasterActionMessageTargets())
					{
						if (answerTarget == out->id())
						{
							*out << answerId;
						}
						else
						{
							*out << TransactionId(0);
						}
						*out << RESET;
						this->writeState(*out);
						out->endMessage();
					}
				}
				this->activate(wasActive);
				if (wasActive)
				{
					mSignal.emit(it, AFTER | RESET);
				}
			}

		private:

			SignalSlot::Signal<const iterator &, int> mSignal;

			std::list<Transaction> mTransactions;
			std::list<std::pair<TransactionId, std::pair<ParticipantId, TransactionId>>> mPassTransactions;

			TransactionId mTransactionCounter;
		};
	}
}
