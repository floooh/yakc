//------------------------------------------------------------------------------
//  Util.cc
//------------------------------------------------------------------------------
#include "Util.h"
#include "IMUI/IMUI.h"
#include <stdio.h>

namespace YAKC {

static const int editFlags = ImGuiInputTextFlags_CharsHexadecimal|
    ImGuiInputTextFlags_CharsUppercase|
    ImGuiInputTextFlags_EnterReturnsTrue;

//------------------------------------------------------------------------------
bool
Util::InputHex8(const char* label, uint8_t& val) {
    bool retval = false;
    static char buf[3];
    UByteToStr(val, buf, sizeof(buf));
    ImGui::PushItemWidth(22);
    if (ImGui::InputText(label, buf, sizeof(buf), editFlags)) {
        val = ParseUByte(buf, val);
        retval = true;
    }
    ImGui::PopItemWidth();
    return retval;
}

//------------------------------------------------------------------------------
bool
Util::InputHex16(const char* label, uint16_t& val) {
    bool retval = false;
    static char buf[5];
    UWordToStr(val, buf, sizeof(buf));
    ImGui::PushItemWidth(38);
    if (ImGui::InputText(label, buf, sizeof(buf), editFlags)) {
        val = ParseUWord(buf, val);
        retval = true;
    }
    ImGui::PopItemWidth();
    return retval;
}

//------------------------------------------------------------------------------
char
Util::NibbleToStr(uint8_t n) {
    return "0123456789ABCDEF"[n & 0xF];
}

//------------------------------------------------------------------------------
void
Util::UByteToStr(uint8_t b, char* buf, int buf_size) {
    o_assert_dbg(buf_size >= 3);
    buf[0] = NibbleToStr(b>>4);
    buf[1] = NibbleToStr(b);
    buf[2] = 0;
}

//------------------------------------------------------------------------------
void
Util::UWordToStr(uint16_t w, char* buf, int bufSize) {
    o_assert(bufSize >= 5);
    UByteToStr(w >> 8, buf, bufSize); buf+=2; bufSize-=2;
    UByteToStr(w & 0xFF, buf, bufSize);
}

//------------------------------------------------------------------------------
uint16_t
Util::ParseUWord(const char* str, uint16_t oldVal) {
    int res = 0;
    if (sscanf(str, "%X", &res) == 1) {
        return (uint16_t) res;
    }
    else {
        return oldVal;
    }
}

//------------------------------------------------------------------------------
uint8_t
Util::ParseUByte(const char* str, uint8_t oldVal) {
    int res = 0;
    if (sscanf(str, "%X", &res) == 1) {
        return (uint8_t) res;
    }
    else {
        return oldVal;
    }
}

//------------------------------------------------------------------------------
ImVec4
Util::RGBA8toImVec4(uint32_t c) {
    ImVec4 v;
    v.x = float(c & 0xFF) / 255.0f;
    v.y = float((c>>8) & 0xFF) / 255.0f;
    v.z = float((c>>16) & 0xFF) / 255.0f;
    v.w = float((c>>16) & 0xFF) / 255.0f;
    return v;
}

} // namespace YAKC
