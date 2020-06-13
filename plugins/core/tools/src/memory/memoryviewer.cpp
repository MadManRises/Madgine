#include "../toolslib.h"

#if ENABLE_MEMTRACKING

#include "memoryviewer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imguiaddons.h"

#include "../renderer/imroot.h"

#include "Modules/keyvalue/scopebase.h"

//#include "Modules/math/math.h"

#include "Interfaces/debug/memory/memory.h"

#include "Interfaces/debug/memory/untrackedmemoryresource.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"
#include "Modules/reflection/classname.h"

UNIQUECOMPONENT(Engine::Tools::MemoryViewer);



namespace Engine
{
	namespace Tools
	{


		static const char *GetSortDirectionLabel(bool sortSelected, bool descending)
		{
			if (!sortSelected)
			{
				return "";
			}

			return descending ? "v" : "^";
		}

		MemoryViewer::MemoryViewer(ImRoot & root) :
			Tool<MemoryViewer>(root),
			mTracker(Debug::Memory::MemoryTracker::getSingleton())
		{
		}

		bool MemoryViewer::traceLevel(const Engine::Debug::TraceBack &traceback, size_t size, bool leaf) {
			ImGui::BeginSpanningTreeNode(&traceback, traceback.mFunction, leaf ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None);

			ImGui::NextColumn();
			if (mShowAddress) {
				ImGui::Text("%#010x", traceback.mAddress);
				ImGui::NextColumn();
			}
			if (mShowFile) {
				ImGui::Text("%s", traceback.mFile);
				ImGui::NextColumn();
				ImGui::Text("%d", traceback.mLineNr);
				ImGui::NextColumn();
			}
			ImGui::Text("%zu", size);
			ImGui::NextColumn();

			return ImGui::EndSpanningTreeNode();

		}

		void MemoryViewer::drawBlock(const BlockData & block, size_t depth)
		{
			for (BlockData *b = block.mFirstChild; b; b = b->mNext) {
				if (traceLevel(b->mFront->first[depth], b->mSize, depth >= b->mFront->first.size() - 1)) {
					drawBlock(*b, depth + 1);
					ImGui::TreePop();
				}
			}
		}

		void MemoryViewer::traceDraw(const std::pmr::vector<Engine::Debug::TraceBack> &data, size_t size, int depth) {
			if (traceLevel(data[depth], size, depth >= data.size() - 1))
			{			
				traceDraw(data, size, depth+1);
				ImGui::TreePop();
			}
		}

		bool MemoryViewer::drawHeader(const char * title, SortMode mode, float size)
		{
			bool result = false;

			bool sortSelected = (mSortMode == mode);

			char buffer[512];

			sprintf(buffer, title, GetSortDirectionLabel(sortSelected, mSortDescending));

			if (size > 0)
				ImGui::SetColumnWidth(-1, size);		

			if (ImGui::Selectable(buffer, sortSelected)) {
				mSortDescending = sortSelected ? !mSortDescending : false;
				mSortMode = mode;
				result = true;
			}			
			if (ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[1]) {
				ImGui::OpenPopup("HeaderPopup");
			}
			ImGui::NextColumn();
			return result;
		}

		static int methodNameCmp(const Item& first, const Item& second) 
		{
			int i;
			for (i = 0; i < first.first.size(); ++i) {
				if (i >= second.first.size())
					return i + 1;
				int comp = strcmp(first.first[i].mFunction, second.first[i].mFunction);
				if (comp != 0)
					return comp * (i + 1);
			}
			if (i < second.first.size())
				return -(i + 1);
			return 0;
		}

		static int methodNameSorting(const Item& first, const Item& second)
		{		
			return sign(methodNameCmp(first, second));			
		}

		static int addressSorting(const Item& first, const Item& second)
		{
			int i;
			for (i = 0; i < first.first.size(); ++i) {
				if (i >= second.first.size())
					return 1;
				if (first.first[i].mAddress < second.first[i].mAddress) return -1;
				if (first.first[i].mAddress > second.first[i].mAddress) return 1;				
			}
			if (i < second.first.size())
				return -1;
			return 0;
		}

		static int fileSorting(const Item& first, const Item& second)
		{
			int i;
			for (i = 0; i < first.first.size(); ++i) {
				if (i >= second.first.size())
					return 1;
				int comp = strcmp(first.first[i].mFile, second.first[i].mFile);
				if (comp != 0)
					return comp;
				if (first.first[i].mLineNr < second.first[i].mLineNr) return -1;
				if (first.first[i].mLineNr > second.first[i].mLineNr) return 1;
			}
			if (i < second.first.size())
				return -1;
			return 0;
		}

		static int sizeSorting(const Item& first, const Item& second)
		{
			if (first.second.mSize > second.second.mSize) return 1;
			if (first.second.mSize < second.second.mSize) return -1;
			return 0;
		}

		static int blockSizeSorting(size_t first, size_t second)
		{
			if (first > second) return 1;
			if (first < second) return -1;
			return 0;
		}

		static int noSorting(const Item& first, const Item& second)
		{
			return 0;
		}

		static void sort(int (*inOrder)(const Item &, const Item &), bool descending, Item *&front) {
			bool sorting = true;
			while (sorting) {
				sorting = false;

				Item **it1 = &front;

				while (*it1) {
					Item **it2 = &(*it1)->second.mNext;

					if (*it2) {
						if (inOrder(**it1, **it2) == (descending ? -1 : 1)) {
							std::swap(*it1, *it2);
							std::swap((*it1)->second.mNext, *it2);
							sorting = true;
						}
					}

					it1 = &(*it1)->second.mNext;
				}

			}
		}

		static void merge(int(*inOrder)(const Item &, const Item &), bool descending, Item *&target, Item *source) {
			Item *p1 = source;
			Item **p2 = &target;
			while (p1 && *p2) {
				if (inOrder(*p1, **p2) == (descending ? -1 : 1)) {
					Item *newItem = p1;
					p1 = p1->second.mNext;
					newItem->second.mNext = *p2;
					*p2 = newItem;
				}
				p2 = &(*p2)->second.mNext;
			}
			if (!*p2) {

				*p2 = p1;
			}
		}

		static void collapseSort(int(*inOrder)(const Item &, const Item &), bool descending, size_t depth, MemoryBlock &block) {

			//assert(block.mSources);

			while (block.mSources) {
				if (block.mSources->first.size() <= depth) {
					assert(block.mSources->first.size() == depth);
					BlockData *tmp = &block.mLeafs.emplace_back(block.mSources, &block.mSources->second.mNext, nullptr, block.mSources->second.mSize);
					tmp->mNext = block.mData.mFirstChild;
					block.mData.mFirstChild = tmp;
					Item *tmp2 = block.mSources;
					block.mSources = block.mSources->second.mNext;
					tmp2->second.mNext = nullptr;
				}
				else {
					auto pib = block.mChildren.try_emplace(block.mSources->first[depth].mAddress, std::make_unique<MemoryBlock>(block.mSources));
					Item *tmp = block.mSources;
					if (pib.second) {
						pib.first->second->mData.mNext = block.mData.mFirstChild;
						block.mData.mFirstChild = &pib.first->second->mData;
						block.mSources = block.mSources->second.mNext;
						tmp->second.mNext = nullptr;
					}
					else {
						block.mSources = block.mSources->second.mNext;
						tmp->second.mNext = pib.first->second->mSources;
						pib.first->second->mSources = tmp;
					}
				}
			}

			for (std::pair<void *const, std::unique_ptr<MemoryBlock>> &p : block.mChildren)
			{
				collapseSort(inOrder, descending, depth + 1, *p.second);
			}

			bool sorting = true;
			while (sorting) {
				sorting = false;

				BlockData **it1 = &block.mData.mFirstChild;

				while (*it1) {
					BlockData **it2 = &(*it1)->mNext;

					if (*it2) {
						int order;
						if (inOrder == sizeSorting) {
							order = blockSizeSorting((*it1)->mSize, (*it2)->mSize);
						}
						else {
							order = inOrder(*(*it1)->mFront, *(*it2)->mFront);
						}
						if (order == (descending ? -1 : 1)) {
							std::swap(*it1, *it2);
							std::swap((*it1)->mNext, *it2);							
							sorting = true;
						}
					}
					it1 = &(*it1)->mNext;
				}
			}

			BlockData *it1 = block.mData.mFirstChild;

			block.mData.mSize = 0;

			while (it1) {
				block.mData.mSize += it1->mSize;
				BlockData *it2 = it1->mNext;
				if (it2)
				{
					*it1->mBack = it2->mFront;
				}
				else {
					block.mData.mBack = it1->mBack;					
				}
				it1 = it2;
			}

			if (block.mData.mFirstChild)
				block.mData.mFront = block.mData.mFirstChild->mFront;
			else
				block.mData.mFront = nullptr;

		}

		static Item *uncollapse(MemoryBlock &block, Item *next) {
			for (BlockData &leaf : block.mLeafs) {
				leaf.mFront->second.mNext = next;
				next = leaf.mFront;
			}
			for (std::pair<void *const, std::unique_ptr<MemoryBlock>> &p : block.mChildren) {
				next = uncollapse(*p.second, next);
			}
			return next;
		}

		void MemoryViewer::render()
		{
			
			if (ImGui::Begin("Memory Viewer", &mVisible))
			{
				ImGui::Value("Total Memory", static_cast<unsigned int>(mTracker.totalMemory()));
				ImGui::Value("Overhead Memory", static_cast<unsigned int>(mTracker.overhead()));

				bool resort = false;
				resort |= ImGui::Checkbox("Collapsing", &mCollapsing);

				if (ImGui::BeginPopup("HeaderPopup")) {					
					ImGui::Checkbox("Address", &mShowAddress);
					ImGui::Checkbox("File", &mShowFile);
					ImGui::EndPopup();
				}

				float width = ImGui::GetWindowWidth() - 20;
				static float addressWidth = ImGui::CalcTextSize(" 0x00000000 ").x;
				static float lineWidth = ImGui::CalcTextSize(" Line v ").x;
				static float sizeWidth = ImGui::CalcTextSize(" 0000000 ").x;

				ImGui::BeginColumns("cols", 2 + mShowAddress + 2 * mShowFile, ImGuiColumnsFlags_NoPreserveWidths);
				float fixedWidth = (mShowAddress ? addressWidth : 0) + (mShowFile ? lineWidth : 0) + sizeWidth;
								
				resort |= drawHeader("Method %s", METHODNAME_SORTING, mShowFile ? 0 : width - fixedWidth);
				if (mShowAddress)
					resort |= drawHeader("Address %s", ADDRESS_SORTING, addressWidth);
				if (mShowFile) {
					resort |= drawHeader("File %s", FILE_SORTING, width - fixedWidth - ImGui::GetColumnWidth(0));
					resort |= drawHeader("Line %s", FILE_SORTING, lineWidth);
				}
				resort |= drawHeader("Size %s", SIZE_SORTING, sizeWidth);
				ImGui::Separator();

				

				int (*inOrder)(const Item &, const Item&);
				switch (mSortMode) {
				case METHODNAME_SORTING:
					inOrder = methodNameSorting;
					break;
				case SIZE_SORTING:
					inOrder = sizeSorting;
					break;
				case FILE_SORTING:
					inOrder = fileSorting;
					break;
				case ADDRESS_SORTING:
					inOrder = addressSorting;
					break;
				default:
					inOrder = noSorting;
				}
				
				
				Item *front = mTracker.linkedFront();
				Item *sortedFront = nullptr;
				Item *unsortedFront = nullptr;

				if (mSortMode == NO_SORTING && !mCollapsing) {
					sortedFront = uncollapse(mRootBlock, front);
					mRootBlock.mChildren.clear();
					mRootBlock.mLeafs.clear();
					mRootBlock.mSources = nullptr;
					mRootBlock.mData = BlockData{};
				}
				else if (resort) {
					if (!mCollapsing) {
						unsortedFront = uncollapse(mRootBlock, front);
						mRootBlock.mChildren.clear();
						mRootBlock.mLeafs.clear();
						mRootBlock.mSources = nullptr;
						mRootBlock.mData = BlockData{};
					}
					else {
						unsortedFront = front;
					}
				}
				else {
					sortedFront = front;
					Item **ptr = &sortedFront;
					Item **target = &unsortedFront;
					for (Item *it = *ptr; it; it = *ptr) {
						++it->second.mGeneration;
						if (it->second.mGeneration <= (mSortMode == SIZE_SORTING ? 2 : 1)) {
							*ptr = it->second.mNext;
							*target = it;
							target = &it->second.mNext;
							*target = nullptr;
						}
						else {
							ptr = &it->second.mNext;
						}
					}
				}

				//Sort Unsorted List & Merge
				if (mCollapsing) {
					/*if (resort) {
						mRootBlock.mChildren.clear();
						mRootBlock.mData = BlockData{};
						mRootBlock.mLeafs.clear();
					}*/
					/*sort(methodNameSorting, false, unsortedFront);
					merge(methodNameSorting, false, sortedFront, unsortedFront);*/
					mRootBlock.mSources = unsortedFront;
					collapseSort(inOrder, mSortDescending, 0, mRootBlock);
					//mCollapsing = false;
				}
				else
				{
					sort(inOrder, mSortDescending, unsortedFront);
					merge(inOrder, mSortDescending, sortedFront, unsortedFront);
				}
				
				//Fix Links
				if (front != sortedFront) {
					Item *originalFront = front;
					if (!mTracker.linkedFront().compare_exchange_strong(front, sortedFront)) {
						while (front->second.mNext != originalFront) front = front->second.mNext;
						front->second.mNext = sortedFront;
					}
				}

				if (mCollapsing) {
					drawBlock(mRootBlock.mData, 0);
				}else{
					for (const std::pair<const Debug::FullStackTrace, Debug::Memory::TracedAllocationData> *it = sortedFront; it; it = it->second.mNext) {
						const auto &trace = *it;
						if (!trace.first.empty() && (trace.second.mSize > 0 || trace.second.mGeneration < 64))
							traceDraw(trace.first, trace.second.mSize, 0);
					}
				}

				ImGui::EndColumns();

			}
			ImGui::End();
		}

		const char * MemoryViewer::key() const
		{
			return "Memory Viewer";
		}
	}
}


METATABLE_BEGIN(Engine::Tools::MemoryViewer)
METATABLE_END(Engine::Tools::MemoryViewer)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Tools::MemoryViewer, Engine::Tools::ToolBase)
SERIALIZETABLE_END(Engine::Tools::MemoryViewer)



#endif