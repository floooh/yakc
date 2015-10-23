#-------------------------------------------------------------------------------
#   instructions.py
#   Code generator script for Z80 instructions.
#-------------------------------------------------------------------------------

Version = 1

import genutil

# 8-bit register bit masks
r8 = [
    (0b000, 'B'), 
    (0b001, 'C'),
    (0b010, 'D'),
    (0b011, 'E'),
    (0b100, 'H'),
    (0b101, 'L'),
    (0b111, 'A')
]

#-------------------------------------------------------------------------------
def r8_name(r) :
    return r8[r][1]

#-------------------------------------------------------------------------------
def r8_bits(r) :
    return r8[r][0]

#-------------------------------------------------------------------------------
def add_op(ops, op, src) :
    '''
    Add an opcode with generated source code lines to a
    global opcodes dictionary (the opcode is the key, and
    the src is the value).
    '''
    if op in ops :
        genutil.fmtError('opcode collision!')
    ops[op] = src
    return ops

#-------------------------------------------------------------------------------
def inc_tstates(src, num) :
    src.append('state.T += {};'.format(num))
    return src

#-------------------------------------------------------------------------------
def gen_ld_r_s(ops) :
    '''
    LD r,s
    T-states: 4
    '''
    for r in range(7) :
        for s in range(7) :
            op = 0b01000000 | r8_bits(r)<<3 | r8_bits(s)
            src = ['// LD {},{}'.format(r8_name(r), r8_name(s))]
            src.append('state.{} = state.{};'.format(r8_name(r), r8_name(s)))
            src = inc_tstates(src, 4)
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_r_n(ops) :
    '''
    LD r,n
    T-states: 7
    '''
    for r in range(7) :
        op = 0b00000110 | r8_bits(r)<<3
        src = ['// LD {},n'.format(r8_name(r))]
        src.append('state.{} = mem->r8(state.PC++);'.format(r8_name(r)))
        src = inc_tstates(src, 7)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_r_hl(ops) :
    '''
    LD r,(HL)
    T-states: 7
    '''
    for r in range(7) :
        op = 0b01000110 | r8_bits(r)<<3
        src = ['// LD {},(HL)'.format(r8_name(r))]
        src.append('state.{} = mem->r8(state.HL);'.format(r8_name(r)))
        src = inc_tstates(src, 7)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_a_bc(ops) :
    '''
    LD A,(BC)
    T-states: 7
    '''
    op = 0b00001010
    src = ['// LD A,(BC)']
    src.append('state.A = mem->r8(state.BC);')
    src = inc_tstates(src, 7)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_a_de(ops) :
    ''' 
    LD A,(DE)
    T-states: 7
    '''
    op = 0b00011010
    src = ['// LD A,(DE)']
    src.append('state.A = mem->r8(state.DE);')
    src = inc_tstates(src, 7)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_a_nn(ops) :
    '''
    LD A,(nn)
    T-states: 13
    '''
    op = 0b00111010
    src = ['// LD A,(nn)']
    src.append('state.A = mem->r8(mem->r16(state.PC));')
    src.append('state.PC += 2;')
    src = inc_tstates(src, 13)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_hl_r(ops) :
    '''
    LD (HL),r
    T-states: 7
    '''
    for r in range(7) :
        op = 0b01110000 | r8_bits(r)
        src = ['// LD (HL),{}'.format(r8_name(r))]
        src.append('mem->w8(state.HL, state.{});'.format(r8_name(r)))
        src = inc_tstates(src, 7)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_hl_n(ops) :
    '''
    LD (HL),n
    T-states: 10
    '''
    op = 0b00110110
    src = ['// LD (HL),n']
    src.append('mem->w8(state.HL, mem->r8(state.PC++));')
    src = inc_tstates(src, 10)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_r_ix_d(ops) :
    ''' 
    LD r,(IX+d)
    T-states: 19
    '''
    for r in range(7) :
        op = 0b01000110 | r8_bits(r)<<3
        src = ['// LD {},(IX+d)'.format(r8_name(r))]
        src.append('d = mem->r8(state.PC++);')
        src.append('state.{} = mem->r8(state.IX + d);'.format(r8_name(r)))
        src = inc_tstates(src, 19)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_r_iy_d(ops) :
    '''
    LD r,(IY+d)
    T-states: 19
    '''
    for r in range(7) :
        op = 0b01000110 | r8_bits(r)<<3
        src = ['// LD {},(IY+d)'.format(r8_name(r))]
        src.append('d = mem->r8(state.PC++);')
        src.append('state.{} = mem->r8(state.IY + d);'.format(r8_name(r)))
        src = inc_tstates(src, 19)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_ix_d_r(ops) :
    '''
    LD (IX+d),r
    T-states: 19
    '''
    for r in range(7) :
        op = 0b01110000 | r8_bits(r)
        src = ['// LD (IX+d),{}'.format(r8_name(r))]
        src.append('d = mem->r8(state.PC++);')
        src.append('mem->w8(state.IX + d, state.{});'.format(r8_name(r)))
        src = inc_tstates(src, 19)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_ix_d_n(ops) :
    '''
    LD (IX+d),n
    T-states: 19
    '''
    op = 0b00110110
    src = ['// LD (IX+d),n']
    src.append('d = mem->r8(state.PC++);')
    src.append('mem->w8(state.IX + d, mem->r8(state.PC++));')
    src = inc_tstates(src, 19)
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_iy_d_r(ops) :
    '''
    LD (IY+d),r
    T-states: 19
    '''
    for r in range(7) :
        op = 0b01110000 | r8_bits(r)
        src = ['// LD (IY+d),{}'.format(r8_name(r))]
        src.append('d = mem->r8(state.PC++);')
        src.append('mem->w8(state.IY + d, state.{});'.format(r8_name(r)))
        src = inc_tstates(src, 19)
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_ld_iy_d_n(ops) :
    '''
    LD (IY+d),n
    T-states: 19
    '''
    op = 0b00110110
    src = ['// LD (IY+d),n']
    src.append('d = mem->r8(state.PC++);')
    src.append('mem->w8(state.IX + d, mem->r8(state.PC++));')
    src = inc_tstates(src, 19);
    ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_opcodes() :
    '''
    Generates the single-byte opcode table.
    '''
    ops = {}
    ops = gen_ld_r_s(ops)
    ops = gen_ld_r_n(ops)
    ops = gen_ld_r_hl(ops)
    ops = gen_ld_hl_r(ops)
    ops = gen_ld_hl_n(ops)
    ops = gen_ld_a_bc(ops)
    ops = gen_ld_a_de(ops)
    ops = gen_ld_a_nn(ops)
    ops[0xDD] = []
    ops[0xFD] = []

    return ops

#-------------------------------------------------------------------------------
def gen_dd_opcodes() :
    '''
    Generate the DD-lead-byte opcode table.
    '''
    dd_ops = {}
    dd_ops = gen_ld_r_ix_d(dd_ops)
    dd_ops = gen_ld_ix_d_r(dd_ops)
    dd_ops = gen_ld_ix_d_n(dd_ops)
    return dd_ops

#-------------------------------------------------------------------------------
def gen_fd_opcodes() :
    '''
    Generate the FD-lead-byte opcode table.
    '''
    fd_ops = {}
    fd_ops = gen_ld_r_iy_d(fd_ops)
    fd_ops = gen_ld_iy_d_r(fd_ops)
    fd_ops = gen_ld_iy_d_n(fd_ops)
    return fd_ops

#-------------------------------------------------------------------------------
def gen_source(f, ops, dd_ops, fd_ops) :
    '''
    Generate the source code from opcode table.
    '''
    
    # write header
    f.write('#pragma once\n')
    f.write('// #version:{}\n'.format(Version))
    f.write('// machine generated, do not edit!\n')
    f.write('#include "yakc/common.h"\n')
    f.write('namespace yakc {\n')
    f.write('inline void z80::step() {\n')
    f.write('    int d;\n')
    f.write('    switch (mem->r8(state.PC++)) {\n')
    for op in ops :
        f.write('    case {}:\n'.format(hex(op)))
        if op == 0xDD :
            # write DD prefix instructions
            f.write('        switch (mem->r8(state.PC++)) {\n')
            for dd_op in dd_ops :
                f.write('        case {}:\n'.format(hex(dd_op)))
                for line in dd_ops[dd_op] :
                    f.write('            {}\n'.format(line))
                f.write('            break;\n')
            f.write('        default:\n')
            f.write('             YAKC_ASSERT(false);\n')
            f.write('        }\n')
        elif op == 0xFD :
            # write FD prefix instructions
            f.write('        switch (mem->r8(state.PC++)) {\n')
            for fd_op in fd_ops :
                f.write('        case {}:\n'.format(hex(fd_op)))
                for line in fd_ops[fd_op] :
                    f.write('            {}\n'.format(line))
                f.write('            break;\n')
            f.write('        default:\n')
            f.write('            YAKC_ASSERT(false);\n')
            f.write('        }\n')
        else :
            for line in ops[op] :
                f.write('        {}\n'.format(line))
        f.write('        break;\n')
    f.write('    default:\n')
    f.write('       YAKC_ASSERT(false);\n')
    f.write('    }\n')
    f.write('}\n')
    f.write('} // namespace yakc\n')

#-------------------------------------------------------------------------------
def generate(input, out_src, out_hdr) :
    if genutil.isDirty(Version, [input], [out_hdr]) :
        ops = gen_opcodes()
        dd_ops = gen_dd_opcodes()
        fd_ops = gen_fd_opcodes()
        with open(out_hdr, 'w') as f:
            gen_source(f, ops, dd_ops, fd_ops)


