#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::memory
    @brief implements banked Z80 memory mapped to host system memory

    Host system memory 'layers' up to 64 KByte size can be mapped
    to Z80 memory with an 2KByte 'page-size' granularity as read-only
    or read/write. The 2 KByte page size is needed because this is the
    smallest memory block 'unit' required by the KC85/4 video memory
    organization.
    
    Memory-mapping definition happens in 'stacked layers', where lower-priority
    mappings fill address-range holes in the higher priority mappings,
    for example:
    
    +-------------+=============+-------------+-------------+
    |             |             |             |             |   64 KByte RAM expansion
    +-------------+=============+-------------+-------------+
                                              +======+
                                              |      |          8 KByte ROM module
                                              +======+
    +=============+             +=============+      +======+
    |             |             |             |      |      |   base-device memory with holes
    +=============+             +=============+      +======+
   0000          4000          8000          C000   E000
   
    This is a KC85/3 base device with switched-off BASIC ROM, leaving
    memory holes at 4000-7FFF and C000-DFFF. There's an 8 KByte ROM
    expansion module in module slot 08 at C000, and a 64KByte RAM expansion
    at module slot 0C. The ROM module is visible in the hole C000-DFFF,
    and a 16 KByte part of the 64 KByte expansion module is visible at
    the hole 4000-7FFF. 3/4 of the 64KByte expansion memory remains
    culled and is not visible to the CPU.
*/
#include "core/common.h"

namespace yakc {

class memory {
public:
    /// 64 kByte addressable memory
    static const int addr_range = 1<<16;
    static const int addr_mask = addr_range - 1;
    /// number of (2K) pages
    static const int num_pages = 32;
    /// max number of layers
    static const int num_layers = 4;

    /// a memory page mapping description
    struct page {
        static const int shift = 11;
        static const uword size = 1<<shift;
        static const uword mask = size - 1;
        ubyte* ptr = nullptr;
        bool writable = false;
    };

    /// memory mapping layers, layer 0 has highest priority
    page layers[num_layers][num_pages];
    /// the actually visible pages
    page pages[num_pages];
    /// a dummy page for currently unmapped memory
    ubyte unmapped_page[page::size];

    /// constructor
    memory();

    /// map a range of memory
    void map(int layer, uword addr, unsigned int size, ubyte* ptr, bool writable);
    /// unmap a range of memory
    void unmap(int layer, uword addr, unsigned int size);
    /// unmap all memory pages in a mapping layer
    void unmap_layer(int layer);
    /// unmap all memory pages
    void unmap_all();
    /// get the layer index a memory page is mapped to, -1 if unmapped
    int get_mapped_layer_index(int page_index) const;

    /// test if an address is writable
    bool is_writable(uword addr) const;
    /// read a byte at cpu address
    ubyte r8(address addr) const;
    /// read a signed byte at cpu address
    byte rs8(address addr) const;
    /// write a byte to cpu address
    void w8(address addr, ubyte b) const;
    /// read/write access to byte
    ubyte& a8(address addr);
    /// read a word at cpu address
    uword r16(address addr) const;
    /// write a word to cpu address
    void w16(address addr, uword w) const;
    /// write a byte range
    void write(address addr, const ubyte* src, int num) const;

private:
    /// update the CPU-visible mapping
    void update_mapping();
};

//------------------------------------------------------------------------------
inline
memory::memory() {
    YAKC_MEMSET(this->unmapped_page, 0xFF, sizeof(this->unmapped_page));
    this->unmap_all();
    this->update_mapping();
}

//------------------------------------------------------------------------------
inline void
memory::map(int layer, uword addr, unsigned int size, ubyte* ptr, bool writable) {
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
        this->layers[layer][page_index].ptr = ptr + offset;
        this->layers[layer][page_index].writable = writable;
    }
    this->update_mapping();
}

//------------------------------------------------------------------------------
inline void
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
        this->layers[layer][page_index] = page();;
    }
    this->update_mapping();
}

//------------------------------------------------------------------------------
inline void
memory::unmap_layer(int layer) {
    YAKC_ASSERT((layer >= 0) && (layer < num_layers));
    for (auto& p : this->layers[layer]) {
        p = page();
    }
    this->update_mapping();
}

//------------------------------------------------------------------------------
inline void
memory::unmap_all() {
    for (auto& l : this->layers) {
        for (auto& p : l) {
            p = page();
        }
    }
    this->update_mapping();
}

//------------------------------------------------------------------------------
inline void
memory::update_mapping() {
    // for each 8KByte memory page, find the highest priority layer
    // which maps this memory range
    for (int page_index = 0; page_index < num_pages; page_index++) {
        int layer_index;
        for (layer_index = 0; layer_index < num_layers; layer_index++) {
            if (this->layers[layer_index][page_index].ptr) {
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
            this->pages[page_index].ptr = this->unmapped_page;
            this->pages[page_index].writable = false;
        }
    }
}

//------------------------------------------------------------------------------
inline int
memory::get_mapped_layer_index(int page_index) const {
    YAKC_ASSERT((page_index>=0) && (page_index < num_pages));
    for (int layer_index = 0; layer_index < num_layers; layer_index++) {
        if (this->pages[page_index].ptr == this->layers[layer_index][page_index].ptr) {
            return layer_index;
        }
    }
    return -1;
}

//------------------------------------------------------------------------------
inline bool
memory::is_writable(uword addr) const {
    return this->pages[addr>>page::shift].writable;
}

//------------------------------------------------------------------------------
inline ubyte
memory::r8(address addr) const {
    return this->pages[addr>>page::shift].ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline byte
memory::rs8(address addr) const {
    return (char) this->pages[addr>>page::shift].ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline ubyte&
memory::a8(address addr) {
    return this->pages[addr>>page::shift].ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline uword
memory::r16(address addr) const {
    ubyte l = this->r8(addr);
    ubyte h = this->r8(addr+1);
    uword w = h << 8 | l;
    return w;
}

//------------------------------------------------------------------------------
inline void
memory::w8(address addr, ubyte b) const {
    const auto& page = this->pages[addr>>page::shift];
    if (page.writable) {
        page.ptr[addr & page::mask] = b;
    }
}

//------------------------------------------------------------------------------
inline void
memory::w16(address addr, uword w) const {
    this->w8(addr, w & 0xFF);
    this->w8(addr + 1, (w>>8));
}

//------------------------------------------------------------------------------
inline void
memory::write(address addr, const ubyte* src, int num) const {
    for (int i = 0; i < num; i++) {
        this->w8(addr++, src[i]);
    }
}

} // namespace yakc
