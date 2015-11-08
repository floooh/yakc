#pragma once
//------------------------------------------------------------------------------
/**
    @file z80dasm.h
    @brief declarations for z80dasm.h
*/

namespace z80dasm {
typedef unsigned char (*fetch_func)(unsigned short base, int offset, void* userdata);
extern int z80disasm(fetch_func fetch, unsigned short pc, char* buffer, void* userdata);
}
