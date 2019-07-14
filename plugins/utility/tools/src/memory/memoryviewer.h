#pragma once

#if ENABLE_MEMTRACKING

#include "../tool.h"
#include "Interfaces/debug/stacktrace.h"
#include "Interfaces/debug/memory/memory.h"

namespace Engine
{
	namespace Tools
	{
		using Item = const std::pair<const Debug::FullStackTrace, Debug::Memory::TracedAllocationData>;
		
		struct BlockData {
			BlockData(Item *front = nullptr, Item **back = nullptr, BlockData *next = nullptr, size_t size = 0) : mFront(front), mBack(back), mNext(next), mSize(size) {}
			Item *mFront = nullptr;
			Item **mBack = nullptr;
			BlockData *mNext = nullptr;
			size_t mSize = 0;
			BlockData *mFirstChild = nullptr;
		};

		struct MemoryBlock {
			MemoryBlock(Item *source = nullptr) :
				mSources(source) {}
			BlockData mData;
			std::list<BlockData> mLeafs;
			std::unordered_map<void *, std::unique_ptr<MemoryBlock>> mChildren;			
			Item *mSources = nullptr;
		};

		class MemoryViewer : public Tool<MemoryViewer>
		{
		public:
		

			enum SortMode {
				NO_SORTING,
				METHODNAME_SORTING,
				ADDRESS_SORTING,
				FILE_SORTING,
				SIZE_SORTING
			};

			MemoryViewer(ImGuiRoot &root);

			virtual void render() override;

			const char* key() override;

		private:
			void traceDraw(const std::pmr::vector<Engine::Debug::TraceBack> &data, size_t size, int depth);
			bool traceLevel(const Engine::Debug::TraceBack &traceback, size_t size, bool leaf);
			void drawBlock(const BlockData &block, size_t depth);

			bool drawHeader(const char *title, SortMode mode, float size = 0);

		private:
			Debug::Memory::MemoryTracker &mTracker;

			bool mSortDescending = false;
			SortMode mSortMode = NO_SORTING;

			bool mShowAddress = true, mShowFile = true;

			bool mCollapsing = false;

			MemoryBlock mRootBlock;
		};

	}
}

RegisterType(Engine::Tools::MemoryViewer);

#endif