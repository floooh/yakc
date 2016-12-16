//------------------------------------------------------------------------------
//  memory.cc
//------------------------------------------------------------------------------
#include "memory.h"

namespace YAKC {

//------------------------------------------------------------------------------
memory::memory() {
    memset(this->unmapped_page, 0xFF, sizeof(this->unmapped_page));
    this->unmap_all();
}

//------------------------------------------------------------------------------
void
memory::map_rw(int layer, uword addr, unsigned int size, ubyte* read_ptr, ubyte* write_ptr) {
    YAKC_ASSERT((layer >= 0) && (layer < num_layers));
    YAKC_ASSERT((addr & page::mask) == 0);
    YAKC_ASSERT((size & page::mask) == 0);
    YAKC_ASSERT(size <= addr_range);

    const uword num = size>>page::shift;
    YAKC_ASSERT(num <= num_pages);
    for (uword i = 0; i < num; i++) {
        const uword offset = i * page::size;
        const uword page_index = ((addr+offset)&addr_mask) >> page::shift;    // page index will wrap around
        YAKC_ASSERT(page_index < num_pages);
        this->layers[layer][page_index].read_ptr = read_ptr + offset;
        if (nullptr != write_ptr) {
            this->layers[layer][page_index].write_ptr = write_ptr + offset;
        }
        else {
            this->layers[layer][page_index].write_ptr = nullptr;
        }
    }
    this->update_mapping();

}

//------------------------------------------------------------------------------
void
memory::map(int layer, uword addr, unsigned int size, ubyte* ptr, bool writable) {
    if (writable) {
        this->map_rw(layer, addr, size, ptr, ptr);
    }
    else {
        this->map_rw(layer, addr, size, ptr, nullptr);
    }
}

//------------------------------------------------------------------------------
void
memory::unmap(int layer, uword addr, unsigned int size) {
    YAKC_ASSERT((layer >= 0) && (layer < num_layers));
    YAKC_ASSERT((addr & page::mask) == 0);
    YAKC_ASSERT((size & page::mask) == 0);
    YAKC_ASSERT(size <= addr_range);

    uword num = size>>page::shift;
    YAKC_ASSERT(num <= num_pages);
    for (uword i = 0; i < num; i++) {
        const uword offset = i * page::size;
        const uword page_index = ((addr+offset)&addr_mask) >> page::shift;
        YAKC_ASSERT(page_index < num_pages);
        this->layers[layer][page_index] = page();
    }
    this->update_mapping();
}

//------------------------------------------------------------------------------
void
memory::unmap_layer(int layer) {
    YAKC_ASSERT((layer >= 0) && (layer < num_layers));
    for (auto& p : this->layers[layer]) {
        p = page();
    }
    this->update_mapping();
}

//------------------------------------------------------------------------------
void
memory::unmap_all() {
    for (auto& l : this->layers) {
        for (auto& p : l) {
            p = page();
        }
    }
    this->update_mapping();
}

//------------------------------------------------------------------------------
void
memory::update_mapping() {
    // for each memory page, find the highest priority layer
    // which maps this memory range
    for (int page_index = 0; page_index < num_pages; page_index++) {
        int layer_index;
        for (layer_index = 0; layer_index < num_layers; layer_index++) {
            if (this->layers[layer_index][page_index].read_ptr) {
                // found highest-priority layer with valid mapping
                break;
            }
        }
        // set the CPU-visible mapping
        if (layer_index != num_layers) {
            // a valid mapping exists for this page
            this->pages[page_index] = this->layers[layer_index][page_index];
        }
        else {
            // no mapping exists, set to the special 'unmapped page'
            this->pages[page_index].read_ptr = this->unmapped_page;
            this->pages[page_index].write_ptr = nullptr;
        }
    }
}

//------------------------------------------------------------------------------
int
memory::layer(uword addr) const {
    const int page_index = addr>>page::shift;
    for (int layer_index = 0; layer_index < num_layers; layer_index++) {
        if (this->pages[page_index].read_ptr == this->layers[layer_index][page_index].read_ptr) {
            return layer_index;
        }
    }
    return -1;
}

} // namespace YAKC
