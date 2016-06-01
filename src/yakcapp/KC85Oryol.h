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
#include "yakc/common.h"
#include "yakc/memory.h"
#include "yakc/z80.h"
#include "yakc/z80dbg.h"
#include "yakc/z80ctc.h"
#include "yakc/z80pio.h"
#include "yakc/kc85.h"
#include "yakc/kc85_exp.h"
#include "yakc/kc85_video.h"
#include "yakc/kc85_audio.h"
#include "yakc/kc85_snapshot.h"
#include "yakc/z1013.h"
#include "yakc/emu.h"
