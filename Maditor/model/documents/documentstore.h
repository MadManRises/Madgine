#pragma once

#include "document.h"

namespace Maditor {
	namespace Model {

		class DocumentStore : public QObject
		{
			Q_OBJECT
		public:
			DocumentStore() = default;
			DocumentStore(const DocumentStore &) = delete;
			DocumentStore(DocumentStore &&) = delete;

			template <class Doc, class... _Ty>
			Doc *createDocument( _Ty&&... args) {
				Doc *document = new Doc(std::forward<_Ty>(args)...);
				mDocuments.emplace_back(document);
				connect(document, &Doc::destroySignal, this, &DocumentStore::destroyDocument);
				emit documentCreated(document);
				return document;
			}

		signals:
			void documentCreated(Document *doc);
			void documentDestroyed(Document *doc);

		private slots:
			void destroyDocument(Document *document) {
				auto it = std::find_if(mDocuments.begin(), mDocuments.end(), [=](const std::unique_ptr<Document> &doc) {return doc.get() == document; });
				assert(it != mDocuments.end());
				emit documentDestroyed(it->get());
				mDocuments.erase(it);
			}

		private:
			std::list<std::unique_ptr<Document>> mDocuments;

		};

	}
}
