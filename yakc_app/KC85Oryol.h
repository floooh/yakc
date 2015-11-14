#pragma once
//------------------------------------------------------------------------------
/**
    @file KC85Oryol.h
    @brief header wrapper for yakc_core using Oryol macro overrides
*/
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#include "Core/Log.h"
#define YAKC_ASSERT(x) o_assert_dbg(x)
#define YAKC_MALLOC(s) Oryol::Memory::Alloc(s)
#define YAKC_FREE(p) Oryol::Memory::Free(p)
#define YAKC_PRINTF(...) Oryol::Log::Info(__VA_ARGS__)
#include "yakc_core/common.h"
#include "yakc_core/memory.h"
#include "yakc_core/z80.h"
#include "yakc_core/kc85.h"
