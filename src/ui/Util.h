#pragma once
//------------------------------------------------------------------------------
/**
    @class Util
    @brief emulator app utility functions
*/
#include "core/common.h"
#include <stdio.h>

class Util {
public:
    /// convert a single 4-bit nibble to a hex character (0..F)
    static char NibbleToStr(yakc::ubyte n) {
        return "0123456789ABCDEF"[n & 0xF];
    }
    
    /// convert a byte to a hex string
    static void UByteToStr(yakc::ubyte b, char* buf, int buf_size) {
        o_assert(buf_size >= 3);
        buf[0] = NibbleToStr(b>>4);
        buf[1] = NibbleToStr(b);
        buf[2] = 0;
    }

    /// convert an uword to a hex string
    static void UWordToStr(yakc::uword w, char* buf, int bufSize) {
        o_assert(bufSize >= 5);
        UByteToStr(w >> 8, buf, bufSize); buf+=2; bufSize-=2;
        UByteToStr(w & 0xFF, buf, bufSize);
    }

    /// parse 4 hex characters into an uint16_t, return old value if failed
    static yakc::uword ParseUWord(const char* str, yakc::uword oldVal) {
        int res = 0;
        if (sscanf(str, "%X", &res) == 1) {
            return (yakc::uword) res;
        }
        else {
            return oldVal;
        }
    }

    /// parse 2 hex characters into an uint8_t, return old value if failed
    static yakc::ubyte ParseUByte(const char* str, yakc::ubyte oldVal) {
        int res = 0;
        if (sscanf(str, "%X", &res) == 1) {
            return (yakc::ubyte) res;
        }
        else {
            return oldVal;
        }
    }
};
