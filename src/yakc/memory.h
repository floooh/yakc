#pragma once
//------------------------------------------------------------------------------
/**
    @class YAKC::memory
    @brief implements banked Z80 memory mapped to host system memory

    Host system memory 'layers' up to 64 KByte size can be mapped
    to Z80 memory with an 1KByte 'page-size' granularity as read-only
    or read/write. The 1 KByte page size is needed because this is the
    smallest memory block 'unit' required by the emulated systems.

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
#include "yakc/core.h"

namespace YAKC {

class memory {
public:
    /// 64 kByte addressable memory
    static const int addr_range = 1<<16;
    static const int addr_mask = addr_range - 1;

    /// a memory page mapping description
    struct page {
        static const int shift = 10;
        static const uint16_t size = 1<<shift;
        static const uint16_t mask = size - 1;
        uint8_t* read_ptr = nullptr;
        uint8_t* write_ptr = nullptr;
    };
    /// number of pages
    static const int num_pages = addr_range / page::size;
    static_assert(num_pages * page::size == addr_range, "page::size must be 2^N and < 64kByte!");
    /// max number of layers
    static const int num_layers = 4;

    /// memory mapping layers, layer 0 has highest priority
    page layers[num_layers][num_pages];
    /// the actually visible pages
    page pages[num_pages];
    /// a dummy page for currently unmapped memory
    ubyte unmapped_page[page::size];

    /// constructor
    memory();
    /// map a range of memory with identical read/write pointer
    void map(int layer, uint16_t addr, uint32_t size, uint8_t* read_ptr, bool writable);
    /// map a range of memory with different read/write pointers
    void map_rw(int layer, uword addr, uint32_t size, uint8_t* read_ptr, uint8_t* write_ptr);
    /// unmap all memory pages in a mapping layer
    void unmap_layer(int layer);
    /// unmap all memory pages
    void unmap_all();
    /// get the layer index a memory page is mapped to, -1 if unmapped
    int layer(uint16_t addr) const;
    /// map a Z80 address to host memory pointer (read-only)
    const uint8_t* read_ptr(uint16_t addr) const;

    /// test if an address is writable
    bool is_writable(uint16_t addr) const;
    /// read a byte at cpu address
    uint8_t r8(uint16_t addr) const;
    /// read a signed byte at cpu address
    int8_t rs8(uint16_t addr) const;
    /// write a byte to cpu address
    void w8(uint16_t addr, uint8_t b) const;
    /// read a word at cpu address
    uint16_t r16(uint16_t addr) const;
    /// write a word to cpu address
    void w16(uint16_t addr, uint16_t w) const;
    /// write a byte range
    void write(uint16_t addr, const uint8_t* src, int num) const;

private:
    /// update the CPU-visible mapping
    void update_mapping(int page_index);
};

//------------------------------------------------------------------------------
inline const uint8_t*
memory::read_ptr(uint16_t addr) const {
    return this->pages[addr>>page::shift].read_ptr;
}

//------------------------------------------------------------------------------
inline bool
memory::is_writable(uint16_t addr) const {
    return nullptr != this->pages[addr>>page::shift].write_ptr;
}

//------------------------------------------------------------------------------
inline uint8_t
memory::r8(uint16_t addr) const {
    return this->pages[addr>>page::shift].read_ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline int8_t
memory::rs8(uword addr) const {
    return (int8_t) this->pages[addr>>page::shift].read_ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline uint16_t
memory::r16(uint16_t addr) const {
    uint8_t l = this->r8(addr);
    uint8_t h = this->r8(addr+1);
    uint16_t w = h << 8 | l;
    return w;
}

//------------------------------------------------------------------------------
inline void
memory::w8(uint16_t addr, uint8_t b) const {
    const auto& page = this->pages[addr>>page::shift];
    if (page.write_ptr) {
        page.write_ptr[addr & page::mask] = b;
    }
}

//------------------------------------------------------------------------------
inline void
memory::w16(uint16_t addr, uint16_t w) const {
    this->w8(addr, w & 0xFF);
    this->w8(addr + 1, (w>>8));
}

//------------------------------------------------------------------------------
inline void
memory::write(uint16_t addr, const uint8_t* src, int num) const {
    for (int i = 0; i < num; i++) {
        this->w8(addr++, src[i]);
    }
}

} // namespace YAKC
