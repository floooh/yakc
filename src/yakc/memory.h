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
        static const uword size = 1<<shift;
        static const uword mask = size - 1;
        ubyte* read_ptr = nullptr;
        ubyte* write_ptr = nullptr;
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
    void map(int layer, uword addr, unsigned int size, ubyte* read_ptr, bool writable);
    /// map a range of memory with different read/write pointers
    void map_rw(int layer, uword addr, unsigned int size, ubyte* read_ptr, ubyte* write_ptr);
    /// unmap all memory pages in a mapping layer
    void unmap_layer(int layer);
    /// unmap all memory pages
    void unmap_all();
    /// get the layer index a memory page is mapped to, -1 if unmapped
    int layer(uword addr) const;
    /// map a Z80 address to host memory pointer (read-only)
    const ubyte* read_ptr(uword addr) const;

    /// test if an address is writable
    bool is_writable(uword addr) const;
    /// read a byte at cpu address
    ubyte r8(uword addr) const;
    /// read a signed byte at cpu address
    byte rs8(uword addr) const;
    /// write a byte to cpu address
    void w8(uword addr, ubyte b) const;
    /// read/write access to byte
    ubyte& a8(uword addr);
    /// read a word at cpu address
    uword r16(uword addr) const;
    /// write a word to cpu address
    void w16(uword addr, uword w) const;
    /// write a byte range
    void write(uword addr, const ubyte* src, int num) const;

private:
    /// update the CPU-visible mapping
    void update_mapping(int page_index);
};

//------------------------------------------------------------------------------
inline const ubyte*
memory::read_ptr(uword addr) const {
    return this->pages[addr>>page::shift].read_ptr;
}

//------------------------------------------------------------------------------
inline bool
memory::is_writable(uword addr) const {
    return nullptr != this->pages[addr>>page::shift].write_ptr;
}

//------------------------------------------------------------------------------
inline ubyte
memory::r8(uword addr) const {
    return this->pages[addr>>page::shift].read_ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline byte
memory::rs8(uword addr) const {
    return (byte) this->pages[addr>>page::shift].read_ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline ubyte&
memory::a8(uword addr) {
    return this->pages[addr>>page::shift].write_ptr[addr&page::mask];
}

//------------------------------------------------------------------------------
inline uword
memory::r16(uword addr) const {
    ubyte l = this->r8(addr);
    ubyte h = this->r8(addr+1);
    uword w = h << 8 | l;
    return w;
}

//------------------------------------------------------------------------------
inline void
memory::w8(uword addr, ubyte b) const {
    const auto& page = this->pages[addr>>page::shift];
    if (page.write_ptr) {
        page.write_ptr[addr & page::mask] = b;
    }
}

//------------------------------------------------------------------------------
inline void
memory::w16(uword addr, uword w) const {
    this->w8(addr, w & 0xFF);
    this->w8(addr + 1, (w>>8));
}

//------------------------------------------------------------------------------
inline void
memory::write(uword addr, const ubyte* src, int num) const {
    for (int i = 0; i < num; i++) {
        this->w8(addr++, src[i]);
    }
}

} // namespace YAKC
