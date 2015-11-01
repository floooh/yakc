#pragma once
//------------------------------------------------------------------------------
/**
    @file kc85_oryol.h
    @brief header wrapper for yakc_core using Oryol macro overrides
*/
#include "Core/Assertion.h"
#define YAKC_ASSERT(x) o_assert_dbg(x)
#include "yakc_core/common.h"
#include "yakc_core/memory.h"
#include "yakc_core/z80.h"
#include "yakc_core/kc85.h"
