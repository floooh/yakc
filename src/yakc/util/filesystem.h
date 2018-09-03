#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::filesystem
    
    A simple memory filesystem for transferring files between 
    emulated systems and the outside world.
*/
#include "yakc/util/core.h"

namespace YAKC {

class filesystem {
public:
    /// file handle typedef
    typedef int file;
    /// the invalid file handle (must evaluate to false!)
    static const int invalid_file = 0;

    /// clear everything in the filesystem
    void reset();
    /// add a file
    bool add(const char* name, const void* ptr, int num_bytes);
    /// get pointer and size of file
    const void* get(const char* name, int& out_size);
    /// delete a file
    void rm(const char* name);
    /// test if a file exists
    bool exists(const char* name);
    /// find a file entry by name
    file find(const char* name);

    static const int store_size = (1024 * 1024);
    static const int max_num_files = 16;

    struct file_item {
        static const int max_name_size = 128;
        char name[max_name_size] = { };
        bool valid = false;
        int pos = 0;
        int size = 0;
    };

    /// current top-free pos
    int free_pos = 0;

    /// note the first entry is never used (it's the 'invalid file')
    file_item files[max_num_files];
    /// byte store
    uint8_t store[store_size];

    /// allocate storage space, returns pos-index, updates free_pos, return -1 if no free space
    int alloc_store(const void* ptr, int num_bytes);
    /// free storage space for file item, merges holes, updates file items
    bool free_store(file f);
};

} // namespace YAKC



