#include <iostream>
#include <deque>
#include <map>
#include <cmath>
#include <cstring>

using namespace std;

size_t const PageSize = 1024;
size_t const MinBlockSize = 16;

class PageAllocator
{

private:
    enum class PageState
    {
        Free,
        DividedIntoBlocks,
        MultiplePageBlock,
    };
    struct PageDescriptor
    {
        PageState state;
        byte* pageStart;
        byte* firstFreeBlock;
        size_t freeBlocksAmount;
        size_t blockSize;
        size_t pageAmount;
    };

    deque <PageDescriptor*> descriptors;
    byte* memory;
    size_t pageAmountInAllocator;


    size_t CalculateBlockSize(size_t size)
    {
        if (size <= MinBlockSize) return MinBlockSize;
        size_t blockSize = MinBlockSize;
        while (blockSize < size)
        {
            blockSize *= 2;
        }
        return blockSize;
    }

    byte* DivideFreePageIntoBlocks(PageDescriptor* descriptor, size_t blockSize)
    {
        descriptor->blockSize = blockSize;
        descriptor->state = PageState::DividedIntoBlocks;
        auto blockAddr = descriptor->firstFreeBlock;
        descriptor->firstFreeBlock += blockSize;
        descriptor->freeBlocksAmount = PageSize / blockSize - 1;
        return blockAddr;
    }
    byte* CreateMultiPageBlock(deque<PageDescriptor*> selectedDescriptors, size_t pageAmount)
    {
        selectedDescriptors[0]->pageAmount = pageAmount;
        for (auto &descriptor : selectedDescriptors)
        {
            descriptor->state = PageState::MultiplePageBlock;
            descriptor->freeBlocksAmount = 0;
            descriptor->firstFreeBlock = nullptr;
            descriptor->blockSize = PageSize;
        }
        return selectedDescriptors[0]->pageStart;
    }

public:
    PageAllocator(size_t size)
    {
        auto allocatorSize = size - size % PageSize;
        pageAmountInAllocator = allocatorSize / PageSize;
        memory = new byte[allocatorSize];
        descriptors.resize(pageAmountInAllocator);
        for (int i = 0; i < pageAmountInAllocator; i++)
        {
            byte* pageAddr = memory + i * PageSize;
            PageDescriptor* descriptor;
            descriptor->state = PageState::Free;
            descriptor->pageStart = pageAddr + i * PageSize;
            descriptor->firstFreeBlock = pageAddr;
            descriptors[i] = descriptor;
        }
    }

    void* mem_alloc(size_t size)
    {
        if (size <= PageSize / 2)
        {
            auto blockSize = CalculateBlockSize(size);
            for (auto &descriptor : descriptors)
            {
                if (descriptor->state == PageState::DividedIntoBlocks and descriptor->freeBlocksAmount > 0)
                {
                    auto blockAddr = descriptor->firstFreeBlock;
                    descriptor->freeBlocksAmount--;
                    if (descriptor->freeBlocksAmount == 0)
                    {
                        descriptor->firstFreeBlock = nullptr;
                    }
                    else descriptor->firstFreeBlock += blockSize;
                    return blockAddr;
                }
            }

            auto firstFreePageDescriptorIterator = find_if(descriptors.begin(), descriptors.end(), [](PageDescriptor* descriptor)
            {
                return descriptor->state == PageState::Free;
            });

            if (firstFreePageDescriptorIterator == descriptors.end())
            {
                return nullptr;
            }
            else
            {
                return DivideFreePageIntoBlocks(*firstFreePageDescriptorIterator, blockSize);
            }

        }
        else
        {
            size_t pageAmount = ceil((double)size / PageSize);
            deque<PageDescriptor*> selectedDescriptors;
            for(int i = 0; i < descriptors.size(); i++)
            {
                if (descriptors[i]->state == PageState::Free)
                {
                    selectedDescriptors.push_back(descriptors[i]);
                }
                else selectedDescriptors.clear();

                if (selectedDescriptors.size() == pageAmount)
                {
                    return CreateMultiPageBlock(selectedDescriptors, pageAmount);
                }
            }
            return nullptr;
        }
    }

    void mem_free(void* addr)
    {
        auto pageDescriptorIterator = find_if(descriptors.begin(), descriptors.end(), [addr](PageDescriptor* descriptor)
        {
            return (descriptor->pageStart <= addr and addr < descriptor->pageStart + PageSize);
        });
        PageDescriptor* descriptor = *pageDescriptorIterator;

        if (descriptor->state == PageState::DividedIntoBlocks)
        {
            descriptor->firstFreeBlock -= descriptor->blockSize;
            descriptor->freeBlocksAmount--;
            if (descriptor->freeBlocksAmount == 0) descriptor->state = PageState::Free;
        }
        else
        {
            auto pageAmount = descriptor->pageAmount;
            deque<PageDescriptor*> multiPageBlock(pageDescriptorIterator, pageDescriptorIterator + pageAmount);
            for (auto &pageDescriptor : multiPageBlock)
            {
                pageDescriptor->pageAmount = 0;
                pageDescriptor->state = PageState::Free;

            }
        }

    }

    void* mem_realloc(void* addr, size_t size)
    {
        mem_free(addr);
        return mem_alloc(size);
    }

    void mem_dump()
    {
        cout << "Page amount: " << pageAmountInAllocator << endl;
        cout << "Page size: " << PageSize << endl;
        for (int i = 0; i < pageAmountInAllocator; i++)
        {
            cout << "Page " << i + 1 << " | ";
            cout << "Address: " << descriptors[i]->pageStart << " | ";
            auto state = descriptors[i]->state == PageState::Free ? "Free" :
                         descriptors[i]->state == PageState::DividedIntoBlocks ? "DividedIntoBlocks" : "MultiplePageBlock";
            cout << "State: " << state << endl;

            /*auto page = memory + i * PageSize;
            auto header = headers[page];
            auto state = header.state == PageState::Free ? "Free" :
                         header.state == PageState::Divided ? "Divided" : "Multiple";
            auto classSize = header.state == PageState::Divided ? " | Class size: " + to_string(header.blockSize) : "";
            auto freePlace = header.state == PageState::Divided ? " | Free blocks: " + to_string(header.blocks) : "";
            cout << "Page " << page << " " << state << classSize << freePlace << endl;*/
        }
        cout << endl;
    }
};

int main()
{
    auto allocator = PageAllocator(4096);
    auto loc1 = allocator.mem_alloc(102);
    allocator.mem_dump();
    return 0;
}