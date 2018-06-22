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
			std::shared_ptr<Doc> createDocument( _Ty&&... args) {
				std::shared_ptr<Doc> document = std::make_shared<Doc>(std::forward<_Ty>(args)...);
				mDocuments.emplace_back(document);
				connect(document.get(), &Doc::destroySignal, this, &DocumentStore::destroyDocument);
				emit documentCreated(document);
				return document;
			}

		signals:
			void documentCreated(const std::shared_ptr<Document> &doc);
			void documentDestroyed(const std::shared_ptr<Document> &doc);

		private slots:
			void destroyDocument(Document *document) {
				auto it = std::find_if(mDocuments.begin(), mDocuments.end(), [=](const std::shared_ptr<Document> &doc) {return doc.get() == document; });
				assert(it != mDocuments.end());
				emit documentDestroyed(*it);
				mDocuments.erase(it);
			}

		private:
			std::vector<std::shared_ptr<Document>> mDocuments;

		};

	}
}
