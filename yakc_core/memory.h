#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::memory
    @brief memory load/store operations
*/
#include "yakc_core/common.h"

namespace yakc {

class memory {
public:
    /// 64 kByte addressable memory
    static const int address_range = 1<<16;
    /// number of banks
    static const int num_banks = 4;

    // memory bank types
    enum class type {
        none,
        ram,
        rom
    };

    /// the memory banks
    struct bank {
        static const int shift = 14;
        static const uword size = 1<<shift;
        static const uword mask = size - 1;
        ubyte* ptr = nullptr;
        bool writable = false;
    } banks[num_banks];

    /// map a memory bank to a block of real memory
    void map(int bank_index, ubyte* ptr, int size, type t);
    /// unmap a memory bank
    void unmap(int bank_index);
    /// get physical start address of memory bank (can be 0!)
    ubyte* get_bank_ptr(int bank_index) const;
    /// get writable flag of bank at index
    bool is_bank_writable(int bank_index) const;
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
inline void
memory::map(int bank_index, ubyte* ptr, int size, type t) {
    YAKC_ASSERT((bank_index >= 0) && (bank_index < num_banks));
    YAKC_ASSERT(ptr && (bank::size == size) && (type::none != t));
    YAKC_ASSERT(nullptr == this->banks[bank_index].ptr);
    this->banks[bank_index].ptr = ptr;
    this->banks[bank_index].writable = (t == type::ram);
}

//------------------------------------------------------------------------------
inline void
memory::unmap(int bank_index) {
        YAKC_ASSERT((bank_index >= 0) && (bank_index < num_banks));
        this->banks[bank_index].ptr = nullptr;
        this->banks[bank_index].writable = false;
    }

//------------------------------------------------------------------------------
inline ubyte*
memory::get_bank_ptr(int bank_index) const {
    YAKC_ASSERT((bank_index >= 0) && (bank_index < num_banks));
    return this->banks[bank_index].ptr;
}

//------------------------------------------------------------------------------
inline bool
memory::is_bank_writable(int bank_index) const {
    YAKC_ASSERT((bank_index >= 0) && (bank_index < num_banks));
    return this->banks[bank_index].writable;
}

//------------------------------------------------------------------------------
inline ubyte
memory::r8(address addr) const {
    const auto& bank = this->banks[addr >> bank::shift];
    if (bank.ptr) {
        return bank.ptr[addr & bank::mask];
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
inline byte
memory::rs8(address addr) const {
    return (char) this->r8(addr);
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
    const auto& bank = this->banks[addr >> bank::shift];
    if (bank.writable) {
        bank.ptr[addr & bank::mask] = b;
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
