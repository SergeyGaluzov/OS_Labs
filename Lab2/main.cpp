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
        byte* pageAddr = memory;
        for (int i = 0; i < pageAmountInAllocator; i++)
        {
            pageAddr += PageSize;
            PageDescriptor* descriptor = new PageDescriptor;
            descriptor->state = PageState::Free;
            descriptor->pageStart = pageAddr;
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
                if (descriptor->state == PageState::DividedIntoBlocks and descriptor->freeBlocksAmount > 0
                and blockSize <= descriptor->blockSize and blockSize > descriptor->blockSize / 2)
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
            descriptor->freeBlocksAmount++;
            if (descriptor->freeBlocksAmount == PageSize / descriptor->blockSize) descriptor->state = PageState::Free;
        }
        else
        {
            auto pageAmount = descriptor->pageAmount;
            deque<PageDescriptor*> multiPageBlock(pageDescriptorIterator, pageDescriptorIterator + pageAmount);
            for (auto &pageDescriptor : multiPageBlock)
            {
                pageDescriptor->pageAmount = 0;
                pageDescriptor->state = PageState::Free;
                pageDescriptor->blockSize = 0;
                pageDescriptor->firstFreeBlock = pageDescriptor->pageStart;
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
        cout << endl;
        cout << "Page amount: " << pageAmountInAllocator << endl;
        cout << "Page size: " << PageSize << endl << endl;
        for (int i = 0; i < pageAmountInAllocator; i++)
        {
            cout << "Page " << i + 1 << " | ";
            cout << "Address: " << descriptors[i]->pageStart << " | ";
            auto state = descriptors[i]->state == PageState::Free ? "Free" :
                         descriptors[i]->state == PageState::DividedIntoBlocks ? "DividedIntoBlocks" : "MultiplePageBlock";
            cout << "State: " << state;

            if (state != "Free")
            {
                if (state == "DividedIntoBlocks")
                {
                    auto blockSize = descriptors[i]->blockSize;
                    cout << " | " << "Block size: " << blockSize << endl;
                    for (int j = 0; j < PageSize / blockSize; j++)
                    {
                        cout <<'\t' << "Block " << j + 1 <<" | ";
                        auto blockAddr = descriptors[i]->pageStart + j * blockSize;
                        cout << "Address: " << blockAddr << " | ";
                        cout << "Block state: " << (blockAddr < descriptors[i]->firstFreeBlock ? "Not free" : "Free") << endl;
                    }
                    cout << endl;
                    continue;
                }
                else
                {
                    if (descriptors[i]->pageAmount != 0)
                    {
                        cout << " | " << "Pages in multipage block: " << descriptors[i]->pageAmount << endl;
                    }
                    else
                    {
                        cout << endl << endl;
                    }
                    continue;
                }
            }
            cout << endl << endl;
        }
    }
};

int main()
{
    auto allocator = PageAllocator(4096);
    auto loc1 = allocator.mem_alloc(2000);
    auto loc2 = allocator.mem_alloc(200);
    auto loc3 = allocator.mem_alloc(200);
    allocator.mem_free(loc2);
    allocator.mem_dump();
    return 0;
}