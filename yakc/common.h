#pragma once
//------------------------------------------------------------------------------
/**
    @file yakc/common.h
    @brief general definitions
*/
#include <assert.h>

#ifndef YAKC_ASSERT
#define YAKC_ASSERT(x) assert(x)
#endif

namespace yakc {

typedef unsigned char ubyte;
typedef char byte;
typedef unsigned short uword;
typedef short word;
typedef unsigned short address;

}