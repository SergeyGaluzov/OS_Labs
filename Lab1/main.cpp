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
    size_t prev_data_size;
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
        head.prev_data_size = 0;
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
        *block_start = block->head.is_block_free; //0
        *(block_start + sizeof(block->head.is_block_free)) = block->head.data_size; //1
        *(block_start + sizeof(block->head.is_block_free) + sizeof(block->head.data_size)) = block->head.prev_data_size; //9
        block->data = block_start + header_size; //24
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
            if ((bool)memory[i] && (size_t)memory[i + 1] >= size)
            {
                auto data_size_before = (size_t)memory[i + 1];
                Block* block1 = new Block(size);
                insertBlock(block1, memory + i);
                if ((data_size_before - block1->head.data_size) > header_size)
                {
                    Block *block2 = new Block(data_size_before - block1->head.data_size - header_size);
                    block2->head.prev_data_size = block1->head.data_size;
                    insertBlock(block2, memory + i + block1->block_size);
                }
                memory[i] = false;
                return memory + i + block1->block_size + header_size;
            }
            else i += (header_size + (size_t)memory[i + 1]);
        }
        return nullptr;
    }

    void mem_free(void* data_addr)
    {
        auto header_addr = (char*)data_addr - header_size;
        auto prev_header_addr = (char*)data_addr - 2 * header_size - header_addr[9];
        auto next_header_addr = header_addr + header_size + header_addr[1];
    }

    void mem_dump()
    {
        size_t i = 0;
        int counter = 1;
        cout << endl;
        while (i < memory_size && (((int)memory[i] == 1) || ((int)memory[i] == 0)))
        {
            cout << "Block " << counter << ":" << endl;
            cout << "Header address: " << (void*)(memory + i) << endl;
            cout << "Free block: " << (bool)memory[i] << endl;
            cout << "The block's data size: " << (size_t)memory[i+1] << endl;
            cout << "Previous block's data size: " << (size_t)memory[i+9] << endl;
            cout << "---------------------------------" << endl;
            counter++;
            i += (header_size + (size_t)memory[i+1]);
        }
    }

    /*void* mem_realloc(void* addr, size_t size)
    {
        return nullptr;
    }*/
};


int main()
{
    Allocator* a = new Allocator(100);
    auto addr1 = a->mem_alloc(20);
    a->mem_dump();
    return 0;
}
