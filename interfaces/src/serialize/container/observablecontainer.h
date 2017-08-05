#pragma once

#include "../observable.h"
#include "../streams/bufferedstream.h"
#include "serializablecontainer.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {

	
		struct _ContainerPolicy {
			enum RequestMode {
				ALL_REQUESTS,
				NO_REQUESTS
			} mRequestMode;

		};

		struct ContainerPolicy{

			static const constexpr _ContainerPolicy allowAll{_ContainerPolicy::ALL_REQUESTS};
			static const constexpr _ContainerPolicy masterOnly{ _ContainerPolicy::NO_REQUESTS };

		};

		template <class C, const _ContainerPolicy &Config>
		class ObservableContainer : public C, public Observable {
		public:
			typedef size_t TransactionId;

			typedef typename C::ArgsTuple ArgsTuple;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;
			typedef typename C::Type Type;

			

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

			ObservableContainer(const ObservableContainer &other) = default;

			ObservableContainer(ObservableContainer &&other) = default;


			template <class T>
			void operator=(T&& arg) {
				if (isMaster()) {
					C::operator=(std::forward<T>(arg));
					onReset();
				}
				else {
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS) {
						C temp(std::forward<T>(arg));

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

			void onReset(ParticipantId answerTarget = 0, TransactionId answerId = 0) const {
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					if (answerTarget == out->id()) {
						*out << answerId;
					}
					else {
						*out << (TransactionId)0;						
					}
					*out << (ACCEPT | RESET);
					this->writeState(*out);
					out->endMessage();
				}
			}

			void clear() {
				C::clear();
				onClear();
			}

			void onClear(ParticipantId answerTarget = 0, TransactionId answerId = 0) const {
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					if (answerTarget == out->id()) {
						*out << answerId;
					}
					else {
						*out << (TransactionId)0;						
					}
					*out << (ACCEPT | CLEAR);
					out->endMessage();
				}
			}

			template <class... _Ty>
			iterator insert(const iterator &where, _Ty&&... args) {
				iterator it = this->end();
				
				if (isMaster()) {
					it = C::insert(where, std::forward<_Ty>(args)...);
					onInsert(it);
				}
				else {
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS) {
						const Type &temp = UnitCreator<Type>::createTemp(std::forward<_Ty>(args)...);

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
			void insert(T &callback, const iterator &where, _Ty&&... args) {
				if (isMaster()) {
					iterator it = C::insert(callback, where, std::forward<_Ty>(args)...);
					onInsert(it);					
				}
				else {
					if (Config.mRequestMode == _ContainerPolicy::ALL_REQUESTS) {
						Type temp(std::forward<_Ty>(args)...);

						TransactionId id = ++mTransactionCounter;

						BufferedOutStream *out = getSlaveActionMessageTarget();
						*out << id;
						*out << INSERT_ITEM;
						this->write_item(*out, where, temp);
						out->endMessage();

						mTransactions.emplace_back(id, callback);
					}
					else {
						//callback(this->end());
						throw 0;
					}
				}

			}

			iterator erase(const iterator &where) {
				iterator it = this->end();

				if (isMaster()) {
					onRemove(where);
					it = C::erase(where);					
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

			void onInsert(const const_iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0) const {
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					if (answerTarget == out->id()) {
						*out << answerId;	
					}
					else {
						*out << (TransactionId)0;					
					}
					*out << (ACCEPT | INSERT_ITEM);
					this->write_item(*out, it);
					out->endMessage();
				}
			}

			void onRemove(const const_iterator &where, ParticipantId answerTarget = 0, TransactionId answerId = 0) const {
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					if (answerTarget == out->id()) {
						*out << answerId;
					}
					else {
						*out << (TransactionId)0;						
					}
					*out << (ACCEPT | REMOVE_ITEM);
					this->write_iterator(*out, where);
					out->endMessage();
				}
			}

			iterator performOperation(Operations op, SerializeInStream &in, ParticipantId partId, TransactionId id) {
				iterator it = this->end();
				switch (op) {
				case INSERT_ITEM:
					it = this->read_item(in, topLevel());
					C::onInsert(it);
					onInsert(it, partId, id);
					break;
				case REMOVE_ITEM:
					it = this->read_iterator(in);
					onRemove(it);
					it = C::erase(it);
					break;
				case CLEAR:
					C::clear();
					onClear(partId, id);
					break;
				case RESET:
					this->readState(in);
					onReset(partId, id);
					break;
				default:
					throw 0;
				}
				return it;
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
					beginActionResponseMessage(&inout);
					inout << id;
					inout << (op | REJECT);
					inout.endMessage();
				}
				else {

					if (isMaster()) {
						performOperation(op, inout, inout.id(), id);
					}
					else {
						TransactionId newId = ++mTransactionCounter;

						BufferedOutStream *out = getSlaveActionMessageTarget();
						*out << newId;
						*out << op;
						*out << inout;
						out->endMessage();

						mPassTransactions.emplace_back(newId, std::make_pair(inout.id(), id));
					}
				}
			}

			virtual void readAction(SerializeInStream &inout) override {

				TransactionId id;
				inout >> id;

				Operations op;
				inout >> (int&)op;

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

				if (id){
					if (!mTransactions.empty() && mTransactions.front().mId == id) {
						mTransactions.front().mCallback(it, accepted);
						mTransactions.pop_front();
					}
				}

			}

		private:

			std::list<Transaction> mTransactions;
			std::list<std::pair<TransactionId, std::pair<ParticipantId, TransactionId>>> mPassTransactions;

			TransactionId mTransactionCounter;

		};
		
	}
}
