//------------------------------------------------------------------------------
//  mos6502.cc
//------------------------------------------------------------------------------
#include "mos6502.h"
#include "yakc/core/system_bus.h"

namespace YAKC {

//------------------------------------------------------------------------------
//  address mode table
//
uint8_t mos_6502::addr_modes[4][8][8] = {
    // cc = 00
    {
        //---    BIT    JMP    JMP()  STY    LDY    CPY    CPX
        { A____, A____, A____, A____, A____, A_IMM, A_IMM, A_IMM },
        { A____, A_ZER, A____, A____, A_ZER, A_ZER, A_ZER, A_ZER },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A_ZPX, A_ZPY, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A_ABX, A____, A____ },
    },
    // cc = 01
    {
        //ORA    AND    EOR    ADC    STA    LDA    CMP    SBC
        { A_IDX, A_IDX, A_IDX, A_IDX, A_IDX, A_IDX, A_IDX, A_IDX },
        { A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER },
        { A_IMM, A_IMM, A_IMM, A_IMM, A____, A_IMM, A_IMM, A_IMM },
        { A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS },
        { A_IDY, A_IDY, A_IDY, A_IDY, A_IDY, A_IDY, A_IDY, A_IDY },
        { A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPX },
        { A_ABY, A_ABY, A_ABY, A_ABY, A_ABY, A_ABY, A_ABY, A_ABY },
        { A_ABX, A_ABX, A_ABX, A_ABX, A_ABX, A_ABX, A_ABX, A_ABX },
    }
    // cc = 02
    {
        //ASL    ROL    LSR    ROR    STX    LDX    DEC    INC
        { A____, A____, A____, A____, A____, A_IMM, A____, A____ },
        { A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER, A_ZER },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS, A_ABS },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A_ZPX, A_ZPX, A_ZPX, A_ZPX, A_ZPY, A_ZPY, A_ZPX, A_ZPX },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A_ABX, A_ABX, A_ABX, A_ABX, A____, A_ABY, A_ABX, A_ABX },
    }
    // cc = 03
    {
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
        { A____, A____, A____, A____, A____, A____, A____, A____ },
    },
};

//------------------------------------------------------------------------------
mos6502::mos6502() {
    this->init();
}

//------------------------------------------------------------------------------
void
mos6502::init() {
    A = 0; X = 0; Y = 0; S = 0; P = XF; PC = 0;
    OP = 0;
    RW = true;
    Cycle = 0;
    FetchReady = true;
    AddrReady = false;
    ExecReady = false;
    AddrMode = A____;
    AddrStep = 0;
}

//------------------------------------------------------------------------------
void
mos6502::reset() {
    P = (IF | XF);
    S = 0xFD;
    PC = mem.r16(0xFFFC);
    RW = true;
    Cycle = 0;
    FetchReady = true;
    AddrReady = false;
    ExecReady = false;
    AddrMode = A____;
    AddrStep = 0;
}

//------------------------------------------------------------------------------
void
mos6502::step(system_bus* bus) {
    // FIXME: Decode overlaps with Execute from the last instruction
    if (FetchReady) {
        step_fetch(bus);
    }
    if (AddReady) {
        step_addr(bus);
    }
    if (ExecReady) {
        step_exec(bus);
    }
    Cycle++;
}

//------------------------------------------------------------------------------
void
mos6502::step_fetch() {
    // fetch next instruction byte
    ADDR = PC++;
    OP = mem.r8(ADDR);

    // decode:
    //  - determine addressing mode
    //  - determine load/store
    uint8_t cc  = OP & 0x03;
    uint8_t bbb = (OP >> 2) & 0x07;
    uint8_t aaa = (OP >> 5) & 0x07;
    AddrMode = addr_modes[cc][bbb][aaa];
    AddrReady = true;
    FetchReady = false;
    ExecReady = false;
    Cycle = 0;
}

//------------------------------------------------------------------------------
void
mos6502::step_exec() {
    FetchReady = true;
}

//------------------------------------------------------------------------------
void
mos6502::step_addr() {
    if (Cycle > 0) {
        switch (AddrMode) {
            case A_IMM: this->step_addr_imm(); break;
            case A_ZER: this->step_addr_zer(); break;
            case A_ZPX: this->step_addr_zpx(); break;
            case A_ZPY: this->step_addr_zpy(); break;
            case A_ABS: this->step_addr_abs(); break;
            case A_ABX: this->step_addr_abx(); break;
            case A_ABY: this->step_addr_aby(); break;
            case A_IDX: this->step_addr_idx(); break;
            case A_IDY: this->step_addr_idy(); break;
            default: ExecReady = true; break;
        }
        DATA = mem.r8(ADDR);
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_imm() {
    ADDR = PC++;
    ExecReady = true;
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_zer() {
    switch (AddrCycle) {
        case 1: ADDR = PC++; break;
        case 2: ADDR = DATA; ExecReady = true; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_zpx() {
    switch (AddrCycle) {
        case 1: ADDR = PC++; break;
        case 2: ADDR = DATA; break;
        case 3: ADDR = (ADDR + X) & 0x00FF; ExecReady = true; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_zpy() {
    switch (AddrCycle) {
        case 1: ADDR = PC++; break;
        case 2: ADDR = DATA; break;
        case 3: ADDR = (ADDR + Y) & 0x00FF; ExecReady = true; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_addr_abs() {
    switch (AddrCycle) {
        case 1: ADDR = PC++; break;
        case 2: tmp = DATA; ADDR = PC++; break;
        case 3: ADDR = DATA<<8 | tmp; ExecReady = true; break;
    }
}

//------------------------------------------------------------------------------
void
mos6502::step_abx() {
    switch (AddrCycle) {
        case 1: ADDR = PC++; break;
        case 2: tmp = DATA + X; ADDR = PC++; break;
        case 3:
            ADDR = (DATA<<8) | (tmp&0xFF);
            if ((tmp & 0xFF00) == 0x00) {
                // page boundary not crossed, can exit early
                ExecReady = true;
            }
            break;
        case 4:
            // only arrives here if page boundary was crossed
            ADDR = (DATA<<8) + tmp;
            ExecReady = true;
            break;
    }
}

} // namespace YAKC
