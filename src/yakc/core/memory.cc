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
memory::map_rw(int layer, uint16_t addr, uint32_t size, uint8_t* read_ptr, uint8_t* write_ptr, mem_cb cb) {
    YAKC_ASSERT((layer >= 0) && (layer < num_layers));
    YAKC_ASSERT((addr & page::mask) == 0);
    YAKC_ASSERT((size & page::mask) == 0);
    YAKC_ASSERT(size <= addr_range);

    const int num = size>>page::shift;
    YAKC_ASSERT(num <= num_pages);
    for (int i = 0; i < num; i++) {
        const uint16_t offset = i * page::size;
        const uint16_t page_index = ((addr+offset)&addr_mask) >> page::shift;    // page index will wrap around

        const int pre_offset = page_index * page::size;
        YAKC_ASSERT(page_index < num_pages);
        auto& page = this->layers[layer][page_index];
        if (cb) {
            YAKC_ASSERT((!read_ptr) && (!write_ptr));
            // special case memory-mapped-io area, an access to this calls
            // the callback function 'cb', to detect a memory-mapped
            // page, the write_ptr is set to nullptr, and the read_ptr
            // is actually the callback function pointer
            page.read_ptr  = (uint8_t*) cb;
            page.write_ptr = nullptr;
        }
        else {
            // the pointers are 'pre-offsetted' by the upper 6 bits of the 16-bit
            // page-start address, this saves us a masking operation later when
            // when accessing the page
            page.read_ptr = (read_ptr - pre_offset) + offset;
            if (nullptr != write_ptr) {
                page.write_ptr = (write_ptr - pre_offset) + offset;
            }
            else {
                page.write_ptr = this->junk_page - pre_offset;
            }
        }
        this->update_mapping(page_index);
    }
}

//------------------------------------------------------------------------------
void
memory::map(int layer, uint16_t addr, uint32_t size, uint8_t* ptr, bool writable) {
    if (writable) {
        this->map_rw(layer, addr, size, ptr, ptr, nullptr);
    }
    else {
        this->map_rw(layer, addr, size, ptr, nullptr, nullptr);
    }
}

//------------------------------------------------------------------------------
void
memory::map_io(int layer, uint16_t addr, uint32_t size, mem_cb cb) {
    this->map_rw(layer, addr, size, nullptr, nullptr, cb);
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
        const int pre_offset = page_index * page::size;
        this->page_table[page_index].read_ptr = this->unmapped_page - pre_offset;
        this->page_table[page_index].write_ptr = this->junk_page - pre_offset;
    }
}

} // namespace YAKC
