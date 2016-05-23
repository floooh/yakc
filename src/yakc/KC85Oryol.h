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
#include "core/common.h"
#include "core/memory.h"
#include "core/z80.h"
#include "core/z80dbg.h"
#include "core/z80ctc.h"
#include "core/z80pio.h"
#include "kc85/kc85.h"
#include "kc85/kc85_exp.h"
#include "kc85/kc85_video.h"
#include "kc85/kc85_audio.h"
#include "kc85/kc85_snapshot.h"
#include "z1013/z1013.h"
