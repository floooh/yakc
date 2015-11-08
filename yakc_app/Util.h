#pragma once
//------------------------------------------------------------------------------
/**
    @class Util
    @brief emulator app utility functions
*/
#include "KC85Oryol.h"

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
        UByteToStr(w>>8, buf, bufSize); buf+=2; bufSize-=2;
        UByteToStr(w, buf, bufSize);
    }

    /// parse 4 hex characters into an uint16_t, return old value if failed
    static yakc::uword ParseUWord(const char* str, yakc::uword oldVal) {
        yakc::uword res = 0;
        for (int i = 0; i < 4; i++) {
            int shift = (3-i)*4;
            char c = str[i];
            if ((c >= '0') && (c <= '9')) {
                res |= (c-'0') << shift;
            }
            else if ((c >= 'A') && (c <= 'F')) {
                res |= ((c-'A')+10) << shift;
            }
            else {
                // failure
                return oldVal;
            }
        }
        return res;
    }

    /// parse 2 hex characters into an uint8_t, return old value if failed
    static yakc::ubyte ParseUByte(const char* str, yakc::ubyte oldVal) {
        yakc::ubyte res = 0;
        for (int i = 0; i < 2; i++) {
            int shift = (1-i)*4;
            char c = str[i];
            if ((c >= '0') && (c <= '9')) {
                res |= (c-'0') << shift;
            }
            else if ((c >= 'A') && (c <= 'F')) {
                res |= ((c-'A')+10) << shift;
            }
            else {
                // failure
                return oldVal;
            }
        }
        return res;
    }
};
