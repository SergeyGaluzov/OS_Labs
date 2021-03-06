#include <iostream>
#include <memory.h>
using namespace std;
struct Header;
struct Block;

struct Header
{
    bool is_block_free;
    size_t data_size;
    size_t prev_data_size;
};
const size_t HEADER_SIZE = sizeof(Header);
struct Block
{
    Header head;
    byte* data;
    size_t block_size;
    const static size_t DATA_SIZE_OFFSET = sizeof(head.is_block_free);
    const static size_t PREV_DATA_SIZE_OFFSET = sizeof(head.is_block_free) + sizeof(head.data_size);
    Block(size_t size)
    {
        head.is_block_free = true;
        head.data_size = size;
        head.prev_data_size = 0;
        block_size = head.data_size + HEADER_SIZE;
    }
};

class Allocator
{
public:
    byte* memory;
    Block* block;
    int memory_size;
    void insertBlock(Block* block, byte* block_start)
    {
        memcpy(block_start, &block->head.is_block_free, sizeof(block->head.is_block_free));
        memcpy(block_start + block->DATA_SIZE_OFFSET, &block->head.data_size, sizeof(block->head.data_size));
        memcpy(block_start + block->PREV_DATA_SIZE_OFFSET, &block->head.prev_data_size, sizeof(block->head.prev_data_size));
        block->data = block_start + HEADER_SIZE;
    }
    Allocator(size_t size)
    {
        memory = new byte[size];
        memory_size = size;
        block = new Block(size - HEADER_SIZE);
        insertBlock(block, memory);
    }
    void* mem_alloc(size_t size)
    {
        int i = 0;
        while (i < memory_size)
        {
            size_t block_data_size;
            memcpy(&block_data_size, memory + i + block->DATA_SIZE_OFFSET, sizeof(block_data_size));
            if ((bool)memory[i] && block_data_size >= size)
            {
                return split_block(memory + i, size);
            }
            else i += (HEADER_SIZE + block_data_size);
        }
        return nullptr;
    }

    void* split_block(void* header_addr, size_t first_block_data_size)
    {
        byte* old_data_addr = (byte*)header_addr + HEADER_SIZE;
        size_t old_block_data_size, prev_block_data_size;
        memcpy(&old_block_data_size, (byte*)header_addr + block->DATA_SIZE_OFFSET, sizeof(old_block_data_size));
        memcpy(&prev_block_data_size, (byte*)header_addr + block->PREV_DATA_SIZE_OFFSET, sizeof(prev_block_data_size));

        Block* block1 = new Block(first_block_data_size);
        block1->head.is_block_free = false;
        block1->head.prev_data_size = prev_block_data_size;

        int block2_data_size = old_block_data_size - block1->block_size;
        if (block2_data_size <= 0 )
        {
            block1->head.data_size = old_block_data_size;
        }

        insertBlock(block1, (byte*)header_addr);

        if (block2_data_size > 0)
        {
            Block *block2 = new Block(block2_data_size);
            block2->head.prev_data_size = first_block_data_size;
            insertBlock(block2, (byte*)header_addr + block1->block_size);
        }
        return old_data_addr;
    }

    void mem_free(void* data_addr)
    {
        byte* header_addr = (byte*)data_addr - HEADER_SIZE;

        size_t prev_data_size, data_size, next_data_size, prev_prev_data_size, next_next_prev_data_size;
        memcpy(&prev_data_size, header_addr + block->PREV_DATA_SIZE_OFFSET, sizeof(prev_data_size));
        memcpy(&data_size, header_addr + block->DATA_SIZE_OFFSET, sizeof(data_size));

        byte* prev_header_addr;
        byte* next_header_addr;
        byte* next_next_header_addr;
        if (memory <= header_addr - prev_data_size - HEADER_SIZE)
        {
            prev_header_addr = header_addr - prev_data_size - HEADER_SIZE;
        }
        else prev_header_addr = nullptr;

        if (prev_header_addr)
        {
            memcpy(&prev_prev_data_size, prev_header_addr + block->PREV_DATA_SIZE_OFFSET, sizeof(prev_prev_data_size));
        }
        else prev_prev_data_size = 0;

        if (memory + memory_size > header_addr + data_size + HEADER_SIZE)
        {
            next_header_addr = header_addr + data_size + HEADER_SIZE;
        }
        else next_header_addr = nullptr;

        if (next_header_addr)
        {
            if (memory + memory_size > next_header_addr + next_data_size + HEADER_SIZE)
            {
                next_next_header_addr = next_header_addr + next_data_size + HEADER_SIZE;
            }
            else
            {
                next_next_header_addr = nullptr;
                next_next_prev_data_size = 0;
            }

            memcpy(&next_data_size, next_header_addr + block->DATA_SIZE_OFFSET, sizeof(next_data_size));
        }
        else next_data_size = 0;

        if (!prev_header_addr && !next_header_addr)
        {
            *header_addr = (byte)true;
        }
        else
        {
            Block* newBlock;
            if (prev_header_addr && (bool)*prev_header_addr)
            {
                newBlock = new Block(prev_data_size + HEADER_SIZE + data_size);
                newBlock->head.prev_data_size = prev_prev_data_size;
                insertBlock(newBlock, prev_header_addr);
                return;
            }
            if (next_header_addr && (bool)*next_header_addr)
            {
                newBlock = new Block(data_size + HEADER_SIZE + next_data_size);
                newBlock->head.prev_data_size = prev_data_size;
                insertBlock(newBlock, header_addr);
                if (next_next_header_addr)
                {
                    next_next_prev_data_size = newBlock->block_size - HEADER_SIZE;
                    memcpy(&next_next_prev_data_size, next_next_header_addr + block->DATA_SIZE_OFFSET, sizeof(next_next_prev_data_size));
                }
                return;
            }
            *header_addr = (byte)true;
        }

    }

    void mem_dump()
    {
        size_t i = 0;
        int counter = 1;
        cout << endl;
        while (i < memory_size)
        {
            cout << "Block " << counter << ":" << endl;
            cout << "Header address: " << memory + i << endl;
            cout << "Free block: " << (bool)memory[i] << endl;
            size_t block_data_size, prev_block_data_size;
            memcpy(&block_data_size, memory + i + block->DATA_SIZE_OFFSET, sizeof(block_data_size));
            cout << "The block's data size: " << block_data_size << endl;
            memcpy(&prev_block_data_size, memory + i + block->PREV_DATA_SIZE_OFFSET, sizeof(prev_block_data_size));
            cout << "Previous block's data size: " << prev_block_data_size << endl;
            cout << "---------------------------------" << endl;
            counter++;
            i += (HEADER_SIZE + block_data_size);
        }
    }

    void* mem_realloc(void* addr, size_t size)
    {
        byte* header_addr = (byte*)addr - HEADER_SIZE;
        size_t current_data_size;
        memcpy(&current_data_size, header_addr + block->DATA_SIZE_OFFSET, sizeof(block->DATA_SIZE_OFFSET));
        byte* next_header_addr_before_split = header_addr + HEADER_SIZE + current_data_size;
        if (size <= current_data_size)
        {
            byte *data_addr = (byte*)split_block(header_addr, size);
            size_t first_block_data_size;
            memcpy(&first_block_data_size, header_addr + block->DATA_SIZE_OFFSET, sizeof(block->DATA_SIZE_OFFSET));
            byte *second_block_header_addr = header_addr + HEADER_SIZE + first_block_data_size;
            if (next_header_addr_before_split != second_block_header_addr)
            {
                size_t prev_data_size = next_header_addr_before_split - second_block_header_addr - HEADER_SIZE;
                memcpy(next_header_addr_before_split + block->PREV_DATA_SIZE_OFFSET, &prev_data_size, sizeof(prev_data_size));
            }
            mem_free(second_block_header_addr + HEADER_SIZE);
            return data_addr;
        }
        else
        {
            mem_free(addr);
            return mem_alloc(size);
        }
    }
};


int main()
{
    Allocator* a = new Allocator(500);
    auto addr1 = a->mem_alloc(40);
    auto addr2 = a->mem_realloc(addr1, 10);
    a->mem_free(addr2);
    a->mem_dump();
    return 0;
}
