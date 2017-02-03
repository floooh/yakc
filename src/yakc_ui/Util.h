#pragma once
//------------------------------------------------------------------------------
/**
    @class Util
    @brief emulator app utility functions
*/
#include "yakc/core/core.h"

namespace YAKC {

class Util {
public:
    /// draw an 8-bit hex input widget
    static bool InputHex8(const char* label, uint8_t& val);
    /// draw a 16-bit hex input widget
    static bool InputHex16(const char* label, uint16_t& val);
    /// convert a single 4-bit nibble to a hex character (0..F)
    static char NibbleToStr(uint8_t n);
    /// convert a byte to a hex string
    static void UByteToStr(uint8_t b, char* buf, int buf_size);
    /// convert an uword to a hex string
    static void UWordToStr(uint16_t w, char* buf, int bufSize);
    /// parse 4 hex characters into an uint16_t, return old value if failed
    static uint16_t ParseUWord(const char* str, uint16_t oldVal);
    /// parse 2 hex characters into an uint8_t, return old value if failed
    static uint8_t ParseUByte(const char* str, uint8_t oldVal);
};

} // namespace YAKC
