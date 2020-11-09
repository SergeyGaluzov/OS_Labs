#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
const int PAGE_SIZE = 1024;
const int MIN_BLOCK_SIZE = 16;

class PageDescriptor;
size_t calculateBlockSize(size_t size)
{
    size_t blockSize = MIN_BLOCK_SIZE;
    while (blockSize < size)
    {
        blockSize *= 2;
    }
    return blockSize;
}

size_t calculatePageAmountInBlockSize(size_t size)
{
    size_t pageAmount = 1;
    while (pageAmount * PAGE_SIZE < size)
    {
        pageAmount++;
    }
    return pageAmount;
}


enum State
{
    Free,
    DividedOnBlocks,
    BLockPart
};

class Block
{
public:
    int size;
    byte* data;
    bool isFree;
    Block(size_t size)
    {
        this->size = size;
        isFree = true;
    }
};

class PageDescriptor
{
public:
    State pageState;
    int freeBlocksNumber;
    int pageAmount;
    vector <Block*> blocks;
    PageDescriptor()
    {
        pageState = State::Free;
    }
};

void* createMultiPageBlock(vector<PageDescriptor*> &descriptors, int index, int pageAmount)
{
    for (auto i = index; i < pageAmount; i++)
    {
        descriptors[i]->blocks.push_back(new Block(PAGE_SIZE));
        descriptors[i]->blocks[0]->isFree = false;
        descriptors[i]->pageState = State::BLockPart;
    }
    descriptors[index]->pageAmount = pageAmount;
    return &descriptors[index]->blocks[0];
}

class PageAllocator
{
public:
    vector <PageDescriptor*> descriptorsList;
    PageAllocator(size_t size)
    {
        for (auto i = 0; i < size / PAGE_SIZE; i++)
        {
            descriptorsList.push_back(new PageDescriptor());
        }
    }
    void* mem_alloc(size_t size)
    {
        size_t blockSize;
        if (size <= PAGE_SIZE / 2)
        {
            blockSize = calculateBlockSize(size);
            for (auto i = 0; i < descriptorsList.size(); i++)
            {
                if (descriptorsList[i]->pageState == State::Free)
                {
                    for (int j = 0; j < PAGE_SIZE / blockSize; j++)
                    {
                        descriptorsList[i]->blocks.push_back(new Block(blockSize));
                        descriptorsList[i]->freeBlocksNumber++;
                    }
                    descriptorsList[i]->blocks[0]->isFree = false;
                    descriptorsList[i]->freeBlocksNumber--;
                    descriptorsList[i]->pageState = State::DividedOnBlocks;
                    return &descriptorsList[i]->blocks[0];
                }
                else
                {
                    if (descriptorsList[i]->pageState == State::DividedOnBlocks)
                    {
                        for (int j = 0; j < descriptorsList[i]->blocks.size(); j++)
                        {
                            if (descriptorsList[i]->blocks[j]->isFree)
                            {
                                descriptorsList[i]->blocks[j]->isFree = false;
                                descriptorsList[i]->freeBlocksNumber--;
                                return descriptorsList[i]->blocks[j];
                            }
                        }
                    }

                }
            }
            return nullptr;
        }
        else
        {
            size_t pageAmount = calculatePageAmountInBlockSize(size);
            for (int i = 0; i < descriptorsList.size() + 1 - pageAmount; i++)
            {
                bool flag = true;
                for (int j = i; j < i + pageAmount; j++)
                {
                    if (descriptorsList[j]->pageState != State::Free)
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    return createMultiPageBlock(descriptorsList, i, pageAmount);
                }
            }
            return nullptr;
        }
    }
};

int main()
{
    PageAllocator a(1024);
    a.mem_alloc(960);
    return 0;
}