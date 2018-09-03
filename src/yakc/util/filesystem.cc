//------------------------------------------------------------------------------
//  filesystem.cc
//------------------------------------------------------------------------------
#if _MSC_VER && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "filesystem.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
filesystem::reset() {
    for (auto& f : files) {
        f = file_item();
    }
    free_pos = 0;
    clear(store, sizeof(store));
}

//------------------------------------------------------------------------------
filesystem::file
filesystem::find(const char* name) {
    YAKC_ASSERT(name);
    for (int i = 0; i < max_num_files; i++) {
        if (files[i].valid) {
            if (strncmp(name, files[i].name, sizeof(files[i].name)) == 0) {
                return i;
            }
        }
    }
    return invalid_file;
}

//------------------------------------------------------------------------------
bool
filesystem::exists(const char* name) {
    YAKC_ASSERT(name);
    return 0 != find(name);
}

//------------------------------------------------------------------------------
int
filesystem::alloc_store(const void* ptr, int num_bytes) {
    if ((free_pos + num_bytes) > store_size) {
        return -1;
    }
    int pos = this->free_pos;
    free_pos += num_bytes;
    memcpy(&store[pos], ptr, num_bytes);
    return pos;
}

//------------------------------------------------------------------------------
bool
filesystem::free_store(file f) {
    YAKC_ASSERT((f >= 0) && (f < max_num_files));
    if (!files[f].valid) {
        return false;
    }
    int pos = files[f].pos;
    int size = files[f].size;
    YAKC_ASSERT((pos + size) <= free_pos);
    YAKC_ASSERT(free_pos >= size);
    for (file_item& item : files) {
        if (item.valid) {
            if (item.pos > pos) {
                item.pos -= size;
                YAKC_ASSERT(item.pos >= 0);
            }
        }
    }
    const int bytes_to_move = free_pos - (pos + size);
    if (bytes_to_move > 0) {
        memmove(&store[pos], &store[pos+size], bytes_to_move);
    }
    free_pos -= size;
    return true;
}

//------------------------------------------------------------------------------
void
filesystem::rm(const char* name) {
    YAKC_ASSERT(name);
    file h = find(name);
    if (h) {
        free_store(h);
        files[h] = file_item();
    }
}

//------------------------------------------------------------------------------
bool
filesystem::add(const char* name, const void* ptr, int num_bytes) {
    YAKC_ASSERT(name && ptr && (num_bytes > 0));
    // file already exists?
    if (find(name)) {
        return invalid_file;
    }
    // find first free file handle
    file h = invalid_file;
    for (h = 1; h < max_num_files; h++) {
        if (!files[h].valid) {
            break;
        }
    }
    // no free file handle found?
    if (!h) {
        return invalid_file;
    }

    // allocate and copy data
    int pos = alloc_store(ptr, num_bytes);
    if (pos < 0) {
        // not enough space in storage left
        return invalid_file;
    }

    // initialize new file item
    file_item& item = files[h];
    strncpy(item.name, name, sizeof(item.name));
    item.name[sizeof(item.name)-1] = 0;
    item.valid = true;
    item.pos = pos;
    item.size = num_bytes;

    return h;
}

//------------------------------------------------------------------------------
const void*
filesystem::get(const char* name, int& out_size) {
    YAKC_ASSERT(name);
    file h = find(name);
    if (h) {
        out_size = files[h].size;
        return &store[files[h].pos];
    }
    else {
        out_size = 0;
        return nullptr;
    }
}

} // namespace YAKC

