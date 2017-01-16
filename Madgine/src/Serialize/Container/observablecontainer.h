#pragma once

#include "../observable.h"
#include "../Streams/bufferedstream.h"
#include "serializablecontainer.h"

namespace Engine {
	namespace Serialize {

		template <class C>
		class ObservableContainer : public C, public Observable {
		public:
			typedef size_t TransactionId;

			typedef typename C::ArgsTuple ArgsTuple;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;
			typedef typename C::Type Type;

			struct Transaction {
				Transaction(TransactionId id, std::function<void(bool, const iterator &it)> callback) :
					mId(id), mCallback(callback) 
				{}

				TransactionId mId;
				std::function<void(bool, const iterator &it)> mCallback;
			};


			ObservableContainer(SerializableUnit *parent) :
				Observable(parent),
				C(parent),
				mTransactionCounter(0)
			{

			}

			template <class P, class... _Ty>
			ObservableContainer(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				Observable(parent),
				C(parent, factory),
				mTransactionCounter(0)
			{
			}

			void clear() {
				C::clear();
				onClear();
			}

			void onClear() const {
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					*out << CLEAR;
					out->endMessage();
				}
			}

			template <class... _Ty>
			iterator insert_where(const iterator &where, _Ty&&... args) {
				iterator it = end();
				if (isMaster()) {
					it = C::insert_where(where, std::forward<_Ty>(args)...);
					onInsert(it);
				}
				else {
					const Type &temp = UnitCreator<Type>::createTemp(std::forward<_Ty>(args)...);

					BufferedOutStream* out = getSlaveActionMessageTarget();
					*out << (TransactionId)0;
					*out << INSERT_ITEM;
					write_item(*out, where, temp);
					out->endMessage();

				}
				return it;
			}

			template <class... _Ty>
			void insert_where_safe(std::function<void(const iterator &)> callback, const iterator &where, _Ty&&... args) {
				if (isMaster()) {
					iterator it = C::insert_where(where, std::forward<_Ty>(args)...);
					onInsert(it);
					callback(it);
				}
				else {
					Type temp(std::forward<_Ty>(args)...);

					TransactionId id = ++mTransactionCounter;

					BufferedOutStream *out = getSlaveActionMessageTarget();
					*out << id;
					*out << INSERT_ITEM;
					write_item(*out, where, temp);
					out->endMessage();

					mTransactions.emplace_back(id, [=](bool, const iterator &it) {callback(it); });

				}
			}

			void onInsert(const const_iterator &it, BufferedInOutStream *answerTarget = 0, TransactionId answerId = 0) const {
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					if (answerTarget == out) {
						*out << answerId;
						*out << (ACCEPT | INSERT_ITEM);
					}
					else {
						*out << (TransactionId)0;
						*out << (ACCEPT | INSERT_ITEM);
					}
					write_item(*out, it);
					out->endMessage();
				}
			}

			
			// Inherited via Observable
			virtual void readRequest(BufferedInOutStream & inout) override
			{
				bool accepted = true; //Check TODO

				TransactionId id;
				inout >> id;

				Operations op;
				inout >> (int&)op;


				if (!accepted) {
					inout.beginMessage();
					inout << id;
					inout << (op | REJECT);
					inout.endMessage();
				}
				else {

					if (isMaster()) {
						iterator it;
						switch (op) {
						case INSERT_ITEM:
							it = read_item(inout);
							C::onInsert(it);
							onInsert(it, &inout, id);
							break;
						case CLEAR:
							clear();
						}
					}
					else {
						TransactionId newId = ++mTransactionCounter;

						BufferedOutStream *out = getSlaveActionMessageTarget();
						*out << newId;
						*out << op;
						*out << inout;
						out->endMessage();

						std::function<void(bool, const iterator &)> callback;

						mPassTransactions.emplace_back(newId, std::make_pair(&inout, id));
					}
				}
			}

			virtual void readAction(BufferedInOutStream &inout) override {

				TransactionId id;
				inout >> id;

				Operations op;
				inout >> (int&)op;

				bool accepted = (id == 0) || ((op & (~MASK)) == ACCEPT);

				iterator it = end();

				std::pair<BufferedInOutStream*, TransactionId> sender = { 0, 0 };
				if (id) {
					if (!mPassTransactions.empty() && mPassTransactions.front().first == id) {
						sender = mPassTransactions.front().second;
						mPassTransactions.pop_front();
					}
				}

				if (accepted) {	

					switch (op & MASK) {
					case INSERT_ITEM:
						it = read_item(inout);
						C::onInsert(it);
						onInsert(it, sender.first, sender.second);
						break;
					default:
						throw 0;
					}
					
				}

				if (id){
					if (!mTransactions.empty() && mTransactions.front().mId == id) {
						mTransactions.front().mCallback(accepted, it);
						mTransactions.pop_front();
					}
				}

			}



		private:

			std::list<Transaction> mTransactions;
			std::list<std::pair<TransactionId, std::pair<BufferedInOutStream*, TransactionId>>> mPassTransactions;

			TransactionId mTransactionCounter;
		};
		
	}
}