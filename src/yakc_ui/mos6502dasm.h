#pragma once
//------------------------------------------------------------------------------
/**
    @file mos6502dasm.h
*/

namespace mos6502dasm {
typedef unsigned char (*fetch_func)(unsigned short base, int offset, void* userdata);
extern int mos6502disasm(fetch_func fetch, unsigned short pc, char* buffer, void* userdata);
}
