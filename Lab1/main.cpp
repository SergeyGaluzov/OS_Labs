#include <iostream>
using namespace std;

const int OFFSET = 4;

size_t memory_align(size_t memory_size)
{
    return memory_size + memory_size % OFFSET;
}
struct Header
{
    size_t data_size;
    bool is_block_free;
};
const size_t header_size = sizeof(Header);
struct Block
{
    Header head;
    char* data;
    size_t block_size;
    Block(size_t size)
    {
        head.is_block_free = true;
        head.data_size = size;
        block_size = head.data_size + header_size;
    }
};

class Allocator
{
public:
    char* memory;
    Block* block;
    int memory_size;
    void insertBlock(Block* block, char* block_start)
    {
        *block_start = block->head.is_block_free;
        *(block_start + sizeof(block->head.is_block_free)) = block->head.data_size;
        block->data = block_start + sizeof(block->head.is_block_free) + sizeof(block->head.data_size);
    }
    Allocator(size_t size)
    {
        memory = new char[size];
        memory_size = size;
        block = new Block(size - header_size);
        insertBlock(block, memory);
    }
    void* mem_alloc(size_t size)
    {
        int i = 0;
        while (i < memory_size)
        {
            if (memory[i] && memory[i + 1] >= size)
            {
                Block* block1 = new Block(size);
                insertBlock(block1, memory + i);
                if ((memory[i + 1] - size - 2 * header_size) > 0)
                {
                    Block *block2 = new Block(memory[i + 1] - size - 2 * header_size);
                    insertBlock(block2, memory + i + block1->block_size);
                }
                memory[i] = false;
                return memory + i + block1->block_size + header_size;
            }
            else i += header_size + memory[i + 1];
        }
    }
};


int main()
{
    Allocator* a = new Allocator(100);
    auto addr1 = a->mem_alloc(100);
    *(int*)addr1 = 445647382;
    cout << *(int*)addr1 << endl;
    return 0;
} 
