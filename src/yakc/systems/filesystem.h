#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::filesystem
    
    A simple memory filesystem for transferring files between 
    emulated systems and the outside world.
*/
#include "yakc/core/core.h"

namespace YAKC {

class filesystem {
public:
    /// open/close modes
    enum class mode {
        none,
        read,
        write,
    };
    /// file handle typedef
    typedef int file;
    /// the invalid file handle (must evaluate to false!)
    static const int invalid_file = 0;

    /// clear everything in the filesystem
    void reset();
    /// open a file
    file open(const char* name, mode m);
    /// read data from file
    int read(file fp, void* ptr, int num_bytes);
    /// write data to file
    int write(file fp, const void* ptr, int num_bytes);
    /// set current file position, only in read mode!
    bool set_pos(file fp, int pos);
    /// get current file position
    int get_pos(file fp) const;
    /// get size of opened file
    int size(file fp) const;
    /// close a file
    void close(file fp);
    /// delete a file
    void rm(const char* name);
    /// test if a file exists
    bool exists(const char* name);
    /// find a file entry by name
    file find(const char* name);

    static const int max_num_blocks = 256;
    struct block {
        static const int shift = 12;            // 1<<12 = 4 kbyte
        static const int size = (1<<shift);
        static const int mask = size - 1;
        bool free = true;
    };
    block blocks[max_num_blocks];

    static const int max_num_files = 16;
    struct file_item {
        static const int max_name_size = 32;
        char name[max_name_size] = { };
        mode open_mode = mode::none;
        bool valid = false;
        int pos = 0;
        int size = 0;
        int b_index = -1;   // last write block index
        static const int max_blocks = 32;
        static const int max_size = max_blocks * block::size;
        uint8_t blocks[max_blocks] = { };
    };
    /// note the first entry is never used (it's the 'invalid file')
    file_item files[max_num_files];

    uint8_t store[max_num_blocks][block::size];

    /// convert a position to a block index
    static int block_index(int pos);
    /// convert a byte size to a block index (size + block_size-1) / block_size
    static int num_blocks(int size);
    /// allocate a new block, return -1 if no free blocks
    int alloc_block();
    /// free a block
    void free_block(int i);
};

} // namespace YAKC



