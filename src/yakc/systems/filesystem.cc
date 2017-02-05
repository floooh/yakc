//------------------------------------------------------------------------------
//  filesystem.cc
//------------------------------------------------------------------------------
#include "filesystem.h"

namespace YAKC {

//------------------------------------------------------------------------------
void
filesystem::reset() {
    for (auto& b : blocks) {
        b = block();
    }
    for (auto& f : files) {
        f = file_item();
    }
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
    return (bool) find(name);
}

//------------------------------------------------------------------------------
int
filesystem::block_index(int pos) {
    int index = pos >> block::shift;
    YAKC_ASSERT(index < max_num_blocks);
    return index;
}

//------------------------------------------------------------------------------
int
filesystem::num_blocks(int size) {
    int num = (size + (block::size-1)) / block::size;
    YAKC_ASSERT(num <= max_num_blocks);
    return num;
}

//------------------------------------------------------------------------------
int
filesystem::alloc_block() {
    for (int i = 0; i < max_num_blocks; i++) {
        if (blocks[i].free) {
            blocks[i].free = false;
            return i;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
void
filesystem::free_block(int i) {
    YAKC_ASSERT((i >= 0) && (i < max_num_blocks));
    YAKC_ASSERT(!blocks[i].free);
    blocks[i].free = true;
}

//------------------------------------------------------------------------------
void
filesystem::rm(const char* name) {
    YAKC_ASSERT(name);
    file h = find(name);
    if (h) {
        file_item& f = files[h];
        for (int i = 0; i < num_blocks(f.size); i++) {
            free_block(f.blocks[i]);
        }
        f = file_item();
    }
}

//------------------------------------------------------------------------------
filesystem::file
filesystem::open(const char* name, mode m) {
    YAKC_ASSERT(name && m != (mode::none));
    if (mode::read == m) {
        // read from file
        file h = find(name);
        if (!h) {
            // file doesn't exist
            return invalid_file;
        }
        if (files[h].open_mode != mode::none) {
            // file is currently open
            return invalid_file;
        }
        files[h].open_mode = mode::read;
        files[h].pos = 0;
        return h;
    }
    else {
        // write to new file

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
        auto& f = files[h];

        // setup the new file item
        strncpy(f.name, name, sizeof(f.name));
        f.name[sizeof(f.name)-1] = 0;
        f.open_mode = mode::write;
        f.valid = true;
        f.pos = 0;
        f.size = 0;
        f.b_index = -1;
        return h;
    }
}

//------------------------------------------------------------------------------
void
filesystem::close(file h) {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode != mode::none);
    f.open_mode = mode::none;
}

//------------------------------------------------------------------------------
void
filesystem::close_rm(file h) {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode != mode::none);
    f.open_mode = mode::none;
    for (int i = 0; i < num_blocks(f.size); i++) {
        free_block(f.blocks[i]);
    }
    f = file_item();
}

//------------------------------------------------------------------------------
int
filesystem::size(file h) const {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    const auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode != mode::none);
    return f.size;
}

//------------------------------------------------------------------------------
bool
filesystem::eof(file h) const {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    const auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode != mode::none);
    return f.pos == f.size;
}

//------------------------------------------------------------------------------
int
filesystem::get_pos(file h) const {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    const auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode != mode::none);
    return f.pos;
}

//------------------------------------------------------------------------------
bool
filesystem::set_pos(file h, int pos) {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode != mode::none);
    if (f.open_mode != mode::read) {
        return false;
    }
    if ((pos < 0) || (pos > f.size)) {
        return false;
    }
    f.pos = pos;
    return true;
}

//------------------------------------------------------------------------------
int
filesystem::write(file h, const void* ptr, int num_bytes) {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    YAKC_ASSERT(ptr && (num_bytes > 0));
    auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode == mode::write);
    YAKC_ASSERT(f.size == f.pos);
    const int end_pos = f.pos + num_bytes;
    int bytes_written = 0;
    const uint8_t* u8_ptr = (const uint8_t*) ptr;
    for (; f.pos < end_pos; f.pos++, f.size++, bytes_written++) {
        if (f.b_index != block_index(f.pos)) {
            // need to start a new block
            int i = alloc_block();
            if (i < 0) {
                // no more free blocks left
                return bytes_written;
            }
            f.b_index = block_index(f.pos);
            f.blocks[f.b_index] = i;
        }
        store[f.blocks[f.b_index]][f.pos&block::mask] = *u8_ptr++;
    }
    return bytes_written;
}

//------------------------------------------------------------------------------
int
filesystem::read(file h, void* ptr, int num_bytes) {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    YAKC_ASSERT(ptr && (num_bytes > 0));
    auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode == mode::read);
    int end_pos = f.pos + num_bytes;
    if (end_pos > f.size) {
        end_pos = f.size;
    }
    int bytes_read = 0;
    uint8_t* u8_ptr = (uint8_t*) ptr;
    for (; f.pos < end_pos; f.pos++, bytes_read++) {
        *u8_ptr++ = store[f.blocks[block_index(f.pos)]][f.pos & block::mask];
    }
    return bytes_read;
}

//------------------------------------------------------------------------------
uint8_t
filesystem::peek_u8(file h, int rel_pos) {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    const auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode == mode::read && f.size > 0);
    int pos = (f.pos + rel_pos) % f.size;
    return store[f.blocks[block_index(pos)]][pos & block::mask];
}

//------------------------------------------------------------------------------
void
filesystem::skip(file h, int num_bytes) {
    YAKC_ASSERT((h != invalid_file) && (h < max_num_files));
    YAKC_ASSERT(num_bytes > 0);
    auto& f = files[h];
    YAKC_ASSERT(f.valid && f.open_mode == mode::read);
    f.pos += num_bytes;
    if (f.pos > f.size) {
        f.pos = f.size;
    }
}

} // namespace YAKC

