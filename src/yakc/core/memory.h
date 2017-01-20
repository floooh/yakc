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
#include "yakc/core/core.h"

namespace YAKC {

class memory {
public:
    /// 64 kByte addressable memory
    static const int addr_range = 1<<16;
    static const int addr_mask = addr_range - 1;

    /// memory-mapped-io callback function typedef
    typedef uint8_t (*mem_cb)(bool write, uint16_t addr, uint8_t inval);

    /// a memory page mapping description
    struct page {
        static const int shift = 10;
        static const uint16_t size = 1<<shift;
        static const uint16_t mask = size - 1;
        const uint8_t* read_ptr = nullptr;
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
    page page_table[num_pages];
    /// a dummy page for currently unmapped memory
    uint8_t unmapped_page[page::size];
    /// another write-only 'junk' page for writes to ROM areas
    uint8_t junk_page[page::size];

    /// constructor
    memory();
    /// map a range of memory with identical read/write pointer
    void map(int layer, uint16_t addr, uint32_t size, uint8_t* ptr, bool writable);
    /// map a range of memory with different read/write pointers
    void map_rw(int layer, uint16_t addr, uint32_t size, uint8_t* read_ptr, uint8_t* write_ptr, mem_cb cb=nullptr);
    /// map a range of memory to a callback for memory-mapped-io
    void map_io(int layer, uint16_t addr, uint32_t size, mem_cb cb);
    /// unmap all memory pages in a mapping layer
    void unmap_layer(int layer);
    /// unmap all memory pages
    void unmap_all();
    /// map a Z80 address to host memory pointer (read-only)
    const uint8_t* read_ptr(uint16_t addr) const;

    /// read a byte at cpu address, no memory-mapped-io support
    uint8_t r8(uint16_t addr) const;
    /// read a signed byte at cpu address, no memory-mapped-io support
    int8_t rs8(uint16_t addr) const;
    /// write a byte to cpu address, no memory-mapped-io support
    void w8(uint16_t addr, uint8_t b) const;
    /// read a word at cpu address, no memory-mapped-io support
    uint16_t r16(uint16_t addr) const;
    /// write a word to cpu address, no memory-mapped-io support
    void w16(uint16_t addr, uint16_t w) const;

    /// read a byte at cpu address, with memory-mapped-io support
    uint8_t r8io(uint16_t addr) const;
    /// read a signed byte at cpu address, with memory-mapped-io support
    int8_t rs8io(uint16_t addr) const;
    /// write a byte to cpu address, with memory-mapped-io support
    void w8io(uint16_t addr, uint8_t b) const;
    /// read a word at cpu address, with memory-mapped-io support
    uint16_t r16io(uint16_t addr) const;
    /// write a word to cpu address, with memory-mapped-io support
    void w16io(uint16_t addr, uint16_t w) const;

    /// write a byte range (calls w8() internally)
    void write(uint16_t addr, const uint8_t* src, int num) const;

private:
    /// update the CPU-visible mapping
    void update_mapping(int page_index);
};

//------------------------------------------------------------------------------
inline const uint8_t*
memory::read_ptr(uint16_t addr) const {
    const int page_index = addr>>page::shift;
    return this->page_table[page_index].read_ptr + page_index*page::size;
}

//------------------------------------------------------------------------------
inline void
memory::w8(uint16_t addr, uint8_t b) const {
    this->page_table[addr>>page::shift].write_ptr[addr] = b;
}

//------------------------------------------------------------------------------
inline void
memory::w8io(uint16_t addr, uint8_t b) const {
    const auto& page = this->page_table[addr>>page::shift];
    if (page.write_ptr) {
        page.write_ptr[addr] = b;
    }
    else {
        // memory-mapped-io page
        ((mem_cb)page.read_ptr)(true, addr, b);
    }
}

//------------------------------------------------------------------------------
inline uint8_t
memory::r8(uint16_t addr) const {
    return this->page_table[addr>>page::shift].read_ptr[addr];
}

//------------------------------------------------------------------------------
inline uint8_t
memory::r8io(uint16_t addr) const {
    const auto& page = this->page_table[addr>>page::shift];
    if (page.write_ptr) {
        return page.read_ptr[addr];
    }
    else {
        // memory-mapped-io page
        return ((mem_cb)page.read_ptr)(false, addr, 0);
    }
}

//------------------------------------------------------------------------------
inline int8_t
memory::rs8(uint16_t addr) const {
    return (int8_t)this->page_table[addr>>page::shift].read_ptr[addr];
}

//------------------------------------------------------------------------------
inline int8_t
memory::rs8io(uint16_t addr) const {
    const auto& page = this->page_table[addr>>page::shift];
    if (page.write_ptr) {
        return (int8_t)page.read_ptr[addr];
    }
    else {
        // memory-mapped-io page
        return (int8_t)((mem_cb)page.read_ptr)(false, addr, 0);
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
memory::w16io(uint16_t addr, uint16_t w) const {
    this->w8io(addr, w & 0xFF);
    this->w8io(addr + 1, (w>>8));
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
inline uint16_t
memory::r16io(uint16_t addr) const {
    uint8_t l = this->r8io(addr);
    uint8_t h = this->r8io(addr+1);
    uint16_t w = h << 8 | l;
    return w;
}

//------------------------------------------------------------------------------
inline void
memory::write(uint16_t addr, const uint8_t* src, int num) const {
    for (int i = 0; i < num; i++) {
        this->w8(addr++, src[i]);
    }
}

} // namespace YAKC
