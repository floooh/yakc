#pragma once
//------------------------------------------------------------------------------
/**
    @class yakc::memory
    @brief memory load/store operations
*/
#include "yakc/common.h"

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
        type type = type::none;
    } banks[num_banks];

    /// map a memory bank to a block of real memory
    void map(int bank_index, ubyte* ptr, int size, type t) {
        YAKC_ASSERT((bank_index >= 0) && (bank_index < num_banks));
        YAKC_ASSERT(ptr && (bank::size == size));
        YAKC_ASSERT(nullptr == this->banks[bank_index].ptr);
        this->banks[bank_index].ptr = ptr;
        this->banks[bank_index].type = t;
    }
    /// unmap a memory bank
    void unmap(int bank_index) {
        YAKC_ASSERT((bank_index >= 0) && (bank_index < num_banks));
        this->banks[bank_index].ptr = nullptr;
        this->banks[bank_index].type = type::none;
    }
    /// read a byte at cpu address
    ubyte read_ubyte(address addr) const {
        const auto& bank = this->banks[addr >> bank::shift];
        if (bank.ptr) {
            return bank.ptr[addr & bank::mask];
        }
        else {
            return 0;
        }
    }
    /// read a word at cpu address
    uword read_uword(address addr) const {
        ubyte l = this->read_ubyte(addr);
        ubyte h = this->read_ubyte(addr+1);
        uword w = h << 8 | l;
        return w;
    }
};

} // namespace yakc
