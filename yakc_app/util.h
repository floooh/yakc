#pragma once
//------------------------------------------------------------------------------
/**
    @class util
    @brief emulator app utility functions
*/
#include "kc85_oryol.h"

class util {
public:
    /// convert a single 4-bit nibble to a hex character (0..F)
    static char nibble_to_str(yakc::ubyte n) {
        return "0123456789ABCDEF"[n & 0xF];
    }
    
    /// convert a byte to a hex string
    static void ubyte_to_str(yakc::ubyte b, char* buf, int buf_size) {
        o_assert(buf_size >= 3);
        buf[0] = nibble_to_str(b>>4);
        buf[1] = nibble_to_str(b);
        buf[2] = 0;
    }

    /// convert an uword to a hex string
    static void uword_to_str(yakc::uword w, char* buf, int buf_size) {
        o_assert(buf_size >= 5);
        ubyte_to_str(w>>8, buf, buf_size); buf+=2; buf_size-=2;
        ubyte_to_str(w, buf, buf_size);
    }

    /// parse 4 hex characters into an uint16_t, return old value if failed
    static yakc::uword parse_uword(const char* str, yakc::uword old_val) {
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
                return old_val;
            }
        }
        return res;
    }

    /// parse 2 hex characters into an uint8_t, return old value if failed
    static yakc::ubyte parse_ubyte(const char* str, yakc::ubyte old_val) {
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
                return old_val;
            }
        }
        return res;
    }
};
