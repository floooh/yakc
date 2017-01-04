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
memory::map_rw(int layer, uint16_t addr, uint32_t size, uint8_t* read_ptr, uint8_t* write_ptr) {
    YAKC_ASSERT((layer >= 0) && (layer < num_layers));
    YAKC_ASSERT((addr & page::mask) == 0);
    YAKC_ASSERT((size & page::mask) == 0);
    YAKC_ASSERT(size <= addr_range);

    const int num = size>>page::shift;
    YAKC_ASSERT(num <= num_pages);
    for (int i = 0; i < num; i++) {
        const uint16_t offset = i * page::size;
        const uint16_t page_index = ((addr+offset)&addr_mask) >> page::shift;    // page index will wrap around
        YAKC_ASSERT(page_index < num_pages);
        this->layers[layer][page_index].read_ptr = read_ptr + offset;
        if (nullptr != write_ptr) {
            this->layers[layer][page_index].write_ptr = write_ptr + offset;
        }
        else {
            this->layers[layer][page_index].write_ptr = this->junk_page;
        }
        this->update_mapping(page_index);
    }
}

//------------------------------------------------------------------------------
void
memory::map(int layer, uint16_t addr, uint32_t size, uint8_t* ptr, bool writable) {
    if (writable) {
        this->map_rw(layer, addr, size, ptr, ptr);
    }
    else {
        this->map_rw(layer, addr, size, ptr, nullptr);
    }
}

//------------------------------------------------------------------------------
void
memory::unmap_layer(int layer) {
    YAKC_ASSERT((layer >= 0) && (layer < num_layers));
    for (int page_index = 0; page_index < num_pages; page_index++) {
        this->layers[layer][page_index] = page();
        this->update_mapping(page_index);
    }
}

//------------------------------------------------------------------------------
void
memory::unmap_all() {
    for (int layer_index = 0; layer_index < num_layers; layer_index++) {
        for (int page_index = 0; page_index < num_pages; page_index++) {
            this->layers[layer_index][page_index] = page();
        }
    }
    for (int page_index = 0; page_index < num_pages; page_index++) {
        this->update_mapping(page_index);
    }
}

//------------------------------------------------------------------------------
void
memory::update_mapping(int page_index) {
    // find the highest priority layer which maps this memory range
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
        this->page_table[page_index] = this->layers[layer_index][page_index];
    }
    else {
        // no mapping exists, set to the special 'unmapped page'
        this->page_table[page_index].read_ptr = this->unmapped_page;
        this->page_table[page_index].write_ptr = this->junk_page;
    }
}

//------------------------------------------------------------------------------
int
memory::layer(uint16_t addr) const {
    const int page_index = addr>>page::shift;
    for (int layer_index = 0; layer_index < num_layers; layer_index++) {
        if (this->page_table[page_index].read_ptr == this->layers[layer_index][page_index].read_ptr) {
            return layer_index;
        }
    }
    return -1;
}

} // namespace YAKC
