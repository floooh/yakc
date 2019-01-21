//------------------------------------------------------------------------------
//  systems.cc
//  Implementation source file for header-only chip and system emulators.
//------------------------------------------------------------------------------
#include "yakc/util/core.h"
#define CHIPS_IMPL
#define CHIPS_ASSERT YAKC_ASSERT
#include "chips/clk.h"
#include "chips/mem.h"
#include "chips/kbd.h"
#include "chips/beeper.h"
#include "chips/z80.h"
#include "chips/z80pio.h"
#include "chips/z80ctc.h"
#include "chips/ay38910.h"
#include "chips/m6502.h"
#include "chips/m6522.h"
#include "chips/i8255.h"
#include "chips/mc6847.h"
#include "chips/mc6845.h"
#include "chips/m6569.h"
#include "chips/m6526.h"
#include "chips/m6581.h"
#include "chips/fdd.h"
#include "chips/fdd_cpc.h"
#include "chips/upd765.h"
#include "chips/am40010.h"
#include "systems/atom.h"
#include "systems/c64.h"
#include "systems/cpc.h"
#include "systems/kc85.h"
#include "systems/z1013.h"
#include "systems/z9001.h"
#include "systems/zx.h"
