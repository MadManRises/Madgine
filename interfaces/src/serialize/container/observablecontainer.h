#pragma once

#include "../observable.h"
#include "../streams/bufferedstream.h"
#include "serializablecontainer.h"
#include "unithelper.h"
#include "sortedcontainer.h"
#include "unsortedcontainer.h"

namespace Engine {
	namespace Serialize {



		enum Operations {
			INSERT_ITEM = 0x1,
			REMOVE_ITEM = 0x2,
			RESET = 0x3,


			MASK = 0x0F,
			ACCEPT = 0x10,
			REJECT = 0x20,

			BEFORE = 0x0,
			AFTER = 0x10
		};

		struct _ContainerPolicy {
			enum RequestMode {
				ALL_REQUESTS,
				NO_REQUESTS
			} mRequestMode;

		};

		struct ContainerPolicy {

			static const constexpr _ContainerPolicy allowAll{ _ContainerPolicy::ALL_REQUESTS };
			static const constexpr _ContainerPolicy masterOnly{ _ContainerPolicy::NO_REQUESTS };

		};

		template <class traits, class Creator>
		using ContainerSelector = std::conditional_t<
			traits::sorted,
			SortedContainer<traits, Creator>,
			UnsortedContainer<traits, Creator>
		>;

		template <class traits, class Creator, const _ContainerPolicy &Config>
		class ObservableContainer :
			public ContainerSelector<traits, Creator>, public Observable {
		public:
			typedef size_t TransactionId;

			typedef ContainerSelector<traits, Creator> Base;

			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;
			typedef typename traits::type type;
			typedef typename traits::value_type value_type;



			struct Transaction {
				template <class T>
				Transaction(TransactionId id, const T &callback) :
					mId(id), mCallback(callback)
				{}

				TransactionId mId;
				SignalSlot::Connection<const iterator &, bool> mCallback;
			};


			ObservableContainer() :
				mTransactionCounter(0)
			{

			}

			ObservableContainer(const ObservableContainer &other) :
				Base(other),
				mTransactionCounter(0)
			{
			}

			ObservableContainer(ObservableContainer &&other) = default;

			virtual ~ObservableContainer() {
				if (unit()->isActive()) {
					mSignal.emit(this->end(), BEFORE | RESET);
					Base::clear();
					mSignal.emit(this->end(), AFTER | RESET);
				}
			}

			template <class T>
			void operator=(T&& arg) {
				if (isMaster()) {
					beforeReset();
					Base::operator=(std::forward<T>(arg));
					afterReset();
				}
				else {
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS) {
						Base temp(std::forward<T>(arg));

						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << (TransactionId)0;
						*out << RESET;
						temp.writeState(*out);
						out->endMessage();
					}
					else {
						throw 0;
					}
				}
			}


			void clear() {
				beforeReset();
				Base::clear();
				afterReset();
			}


			template <class... _Ty>
			iterator insert(const iterator &where, _Ty&&... args) {
				iterator it = this->end();

				if (isMaster()) {
					it = Base::insert(where, std::forward<_Ty>(args)...);
					onInsert(it);
				}
				else {
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS) {
						type temp(std::forward<_Ty>(args)...);
						this->postConstruct(temp);

						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << (TransactionId)0;
						*out << INSERT_ITEM;
						this->write_item(*out, where, temp);
						out->endMessage();
					}
					else {
						throw 0;
					}
				}
				return it;
			}

			template <class T, class... _Ty>
			void insert(T &&init, const iterator &where, _Ty&&... args) {
				if (isMaster()) {
					iterator it = Base::insert(std::forward<T>(init), where, std::forward<_Ty>(args)...);
					onInsert(it);
				}
				else {
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS) {
						type temp(std::forward<_Ty>(args)...);
						this->postConstruct(temp);

						init(temp);

						BufferedOutStream *out = getSlaveActionMessageTarget();
						*out << (TransactionId)0;
						*out << INSERT_ITEM;
						this->write_item(*out, where, temp);
						out->endMessage();
					}
					else {
						throw 0;
					}
				}

			}

			iterator erase(const iterator &where) {
				iterator it = this->end();

				if (isMaster()) {
					beforeRemove(where);
					it = Base::erase(where);
					afterRemove();
				}
				else {
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS) {
						BufferedOutStream* out = getSlaveActionMessageTarget();
						*out << (TransactionId)0;
						*out << REMOVE_ITEM;
						this->write_iterator(*out, where);
						out->endMessage();
					}
					else {
						throw 0;
					}
				}
				return it;
			}



			iterator performOperation(Operations op, SerializeInStream &in, ParticipantId partId, TransactionId id) {
				iterator it = this->end();
				switch (op) {
				case INSERT_ITEM:
					it = this->read_item(in);
					onInsert(it, partId, id);
					break;
				case REMOVE_ITEM:
					it = this->read_iterator(in);
					beforeRemove(it, partId, id);
					it = Base::erase(it);
					afterRemove();
					break;
				case RESET:
					beforeReset();
					Base::readState(in);
					afterReset(partId, id);
					break;
				default:
					throw 0;
				}
				return it;
			}

			virtual void readState(SerializeInStream &in) override {
				beforeReset();
				Base::readState(in);
				afterReset();
			}


			// Inherited via Observable
			virtual void readRequest(BufferedInOutStream & inout) override
			{
				bool accepted = Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS; //Check TODO

				TransactionId id;
				inout >> id;

				Operations op;
				inout >> (int&)op;


				if (!accepted) {
					if (id) {
						beginActionResponseMessage(&inout);
						inout << id;
						inout << (op | REJECT);
						inout.endMessage();
					}
				}
				else {

					if (isMaster()) {
						performOperation(op, inout, inout.id(), id);
					}
					else {
						TransactionId newId = 0;
						if (id) {
							newId = ++mTransactionCounter;
						}
						BufferedOutStream *out = getSlaveActionMessageTarget();
						*out << newId;
						*out << op;
						*out << inout;
						out->endMessage();

						if (id) {
							mPassTransactions.emplace_back(newId, std::make_pair(inout.id(), id));
						}
					}
				}
			}

			virtual void readAction(SerializeInStream &inout) override {

				TransactionId id;
				inout >> id;

				Operations op;
				inout >> op;

				bool accepted = (id == 0) || ((op & (~MASK)) == ACCEPT);

				iterator it = this->end();

				std::pair<ParticipantId, TransactionId> sender = { 0, 0 };
				if (id) {
					if (!mPassTransactions.empty() && mPassTransactions.front().first == id) {
						sender = mPassTransactions.front().second;
						mPassTransactions.pop_front();

					}
				}

				if (accepted) {
					it = performOperation((Operations)(op & MASK), inout, sender.first, sender.second);
				}

				if (id) {
					if (!mTransactions.empty() && mTransactions.front().mId == id) {
						mTransactions.front().mCallback(it, accepted);
						mTransactions.pop_front();
					}
				}

			}

			virtual void setActive(bool b) override {
				if (b) {
					mSignal.emit(this->begin(), BEFORE | RESET);
					mSignal.emit(this->end(), AFTER | RESET);
				}
				Base::setActive(b);
				if (!b) {
					mSignal.emit(this->end(), BEFORE | RESET);
					mSignal.emit(this->begin(), AFTER | RESET);
				}
			}

			template <class Ty>
			void connectCallback(Ty &&slot) {
				mSignal.connect(std::forward<Ty>(slot));
			}


		private:
			void onInsert(const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0) {
				if (unit()->isActive()) {
					for (BufferedOutStream *out : getMasterActionMessageTargets()) {
						if (answerTarget == out->id()) {
							*out << answerId;
						}
						else {
							*out << (TransactionId)0;
						}
						*out << INSERT_ITEM;
						this->write_item(*out, it);
						out->endMessage();
					}
					mSignal.emit(it, INSERT_ITEM);
				}
			}

			void beforeRemove(const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0) {
				if (unit()->isActive()) {
					for (BufferedOutStream *out : getMasterActionMessageTargets()) {
						if (answerTarget == out->id()) {
							*out << answerId;
						}
						else {
							*out << (TransactionId)0;
						}
						*out << REMOVE_ITEM;
						this->write_iterator(*out, it);
						out->endMessage();
					}
					mSignal.emit(it, BEFORE | REMOVE_ITEM);
				}
			}

			void afterRemove() {
				if (unit()->isActive()) {
					mSignal.emit(this->end(), AFTER | REMOVE_ITEM);
				}
			}

			void beforeReset() {
				if (unit()->isActive()) {
					mSignal.emit(this->end(), BEFORE | RESET);
				}
			}

			void afterReset(ParticipantId answerTarget = 0, TransactionId answerId = 0) {
				if (unit()->isActive()) {
					for (BufferedOutStream *out : getMasterActionMessageTargets()) {
						if (answerTarget == out->id()) {
							*out << answerId;
						}
						else {
							*out << (TransactionId)0;
						}
						*out << RESET;
						this->writeState(*out);
						out->endMessage();
					}
					mSignal.emit(this->end(), AFTER | RESET);
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
