#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::memory
    @brief map Z80 memory to host system memory
*/
#include <string.h>
#include "yakc_core/common.h"

namespace yakc {

class memory {
public:
    /// 64 kByte addressable memory
    static const int address_range = 1<<16;
    /// number of (8K) pages
    static const int num_pages = 8;

    /// the memory banks
    struct page {
        static const int shift = 13;
        static const uword size = 1<<shift;
        static const uword mask = size - 1;
        ubyte* ptr = nullptr;
        bool writable = false;
    } pages[num_pages];

    /// a dummy page for unmapped memory
    ubyte unmapped_page[page::size];

    /// constructor
    memory();

    /// map a range of memory
    void map(uword addr, uword size, ubyte* ptr, bool writable);
    /// unmap a range of memory
    void unmap(uword addr, uword size);
    /// unmap all memory pages
    void unmap_all();
    /// test if an address is writable
    bool is_writable(uword addr) const;
    /// read a byte at cpu address
    ubyte r8(address addr) const;
    /// read a signed byte at cpu address
    byte rs8(address addr) const;
    /// read a word at cpu address
    uword r16(address addr) const;
    /// write a byte to cpu address
    void w8(address addr, ubyte b) const;
    /// write a word to cpu address
    void w16(address addr, uword w) const;
    /// write a byte range
    void write(address addr, ubyte* src, int num) const;
};

//------------------------------------------------------------------------------
inline
memory::memory() {
    memset(this->unmapped_page, 0xFF, sizeof(this->unmapped_page));
    this->unmap_all();
}

//------------------------------------------------------------------------------
inline void
memory::map(uword addr, uword size, ubyte* ptr, bool writable) {
    YAKC_ASSERT((addr & page::mask) == 0);
    YAKC_ASSERT((size & page::mask) == 0);
    const uword num = size>>page::shift;
    YAKC_ASSERT(num <= num_pages);
    for (uword i = 0; i < num; i++) {
        const uword offset = i * page::size;
        const uword page_index = (addr+offset)>>page::shift;
        this->pages[page_index].ptr = ptr + offset;
        this->pages[page_index].writable = writable;
    }
}

//------------------------------------------------------------------------------
inline void
memory::unmap(uword addr, uword size) {
    YAKC_ASSERT((addr & page::mask) == 0);
    YAKC_ASSERT((size & page::mask) == 0);
    uword num = size>>page::shift;
    YAKC_ASSERT(num <= num_pages);
    for (uword i = 0; i < num; i++) {
        const uword offset = i * page::size;
        const uword page_index = (addr+offset)>>page::shift;
        this->pages[page_index].ptr = this->unmapped_page;
        this->pages[page_index].writable = false;
    }
}

//------------------------------------------------------------------------------
inline void
memory::unmap_all() {
    for (auto& p : this->pages) {
        p.ptr = this->unmapped_page;
        p.writable = false;
    }
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
memory::write(address addr, ubyte* src, int num) const {
    for (int i = 0; i < num; i++) {
        this->w8(addr++, src[i]);
    }
}

} // namespace yakc
