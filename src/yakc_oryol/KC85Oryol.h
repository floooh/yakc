#pragma once
//------------------------------------------------------------------------------
/**
    @file KC85Oryol.h
    @brief header wrapper for yakc_core using Oryol macro overrides
*/
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#include "Core/Log.h"
#include "yakc/yakc.h"

inline void* oryol_malloc(size_t s) {
    return Oryol::Memory::Alloc((int)s);
}

inline void oryol_free(void* p) {
    Oryol::Memory::Free(p);
}

inline int oryol_printf(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    Oryol::Log::VInfo(msg, args);
    va_end(args);
    return 0;
}

