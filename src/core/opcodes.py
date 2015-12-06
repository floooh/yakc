#-------------------------------------------------------------------------------
#   opcodes.py
#   Code generator script for Z80 instructions.
#-------------------------------------------------------------------------------

Version = 1

from collections import namedtuple
import genutil

# 8-bit register bit masks
R = namedtuple('REG', 'bits name mem')
r8 = [
    R(0b000, 'B', False), 
    R(0b001, 'C', False),
    R(0b010, 'D', False),
    R(0b011, 'E', False),
    R(0b100, 'H', False),
    R(0b101, 'L', False),
    R(0b111, 'A', False)
]

r8ihl = [
    R(0b000, 'B', False), 
    R(0b001, 'C', False),
    R(0b010, 'D', False),
    R(0b011, 'E', False),
    R(0b100, 'H', False),
    R(0b101, 'L', False),
    R(0b110, '(HL)', True),
    R(0b111, 'A', False)
]

r8ixy = [
    R(0b100, 'H', False),
    R(0b101, 'L', False),
]

# 16-bit register bit masks with SP
r16sp = [
    R(0b00, 'BC', False),
    R(0b01, 'DE', False),
    R(0b10, 'HL', False),
    R(0b11, 'SP', False)
]

# 16-bit register bit masks with AF
r16af = [
    R(0b00, 'BC', False),
    R(0b01, 'DE', False),
    R(0b10, 'HL', False),
    R(0b11, 'AF', False)
]

# 16-bit register bit masks with IX/IY instead of HL
r16ixy = {
    'IX': [
        R(0b00, 'BC', False),
        R(0b01, 'DE', False),
        R(0b10, 'IX', False),
        R(0b11, 'SP', False),
    ],
    'IY': [
        R(0b00, 'BC', False),
        R(0b01, 'DE', False),
        R(0b10, 'IY', False),
        R(0b11, 'SP', False),
    ],
}

CC = namedtuple('CC', 'bits name test')
cc = [
    CC(0b000, 'NZ', '(!(state.F & ZF))'),
    CC(0b001, 'Z',  '(state.F & ZF)'),
    CC(0b010, 'NC', '(!(state.F & CF))'),
    CC(0b011, 'C',  '(state.F & CF)'),
    CC(0b100, 'PO', '(!(state.F & PF))'),
    CC(0b101, 'PE', '(state.F & PF)'),
    CC(0b110, 'P',  '(!(state.F & SF))'),
    CC(0b111, 'M',  '(state.F & SF)')
] 

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
def t(num) :
    return 'return {};'.format(num)

#-------------------------------------------------------------------------------
def NOP_HALT_DI_EI(ops) :
    ops = add_op(ops, 0x00, ['// NOP', t(4)])
    ops = add_op(ops, 0x76, ['// HALT', 'halt();', t(4)])
    ops = add_op(ops, 0xF3, ['// DI', 'di();', t(4)])
    ops = add_op(ops, 0xFB, ['// EI', 'ei();', t(4)])
    return ops

#-------------------------------------------------------------------------------
def IM(ops) :
    '''
    IM 0,1,2
    T-states: 8
    '''
    for im in {(0,0x46),(1,0x56),(2,0x5E)} :
        src = [
            '// IM {}'.format(im[0]),
            'state.IM = {};'.format(im[0]),
            t(8)]
        ops = add_op(ops, im[1], src)
    return ops

#-------------------------------------------------------------------------------
def LD_r_s(ops) :
    for r in r8ihl :
        for s in r8ihl :
            op = 0b01000000 | r.bits<<3 | s.bits
            cycles = 4
            if r.mem and s.mem :
                # skip special case LD (HL),(HL) is actually HALT
                continue
            if r.mem :
                cycles = 7
            else :
                lhs = 'state.{}'.format(r.name)
            if s.mem :
                rhs = 'mem.r8(state.HL)'
                cycles = 7
            else :
                rhs = 'state.{}'.format(s.name)
            src = [ '// LD {},{}'.format(r.name, s.name) ]
            if r.mem :
                src.append('mem.w8(state.HL, {});'.format(rhs))
            else :
                src.append('{} = {};'.format(lhs, rhs))
            src.append(t(cycles))
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_r_n(ops) :
    for r in r8ihl :
        op = 0b00000110 | r.bits<<3
        if r.mem :
            cycles = 10
        else :
            lhs = 'state.{}'.format(r.name)
            cycles = 7
        rhs = 'mem.r8(state.PC++)'
        src = [ '// LD {},n'.format(r.name) ]
        if r.mem :
            src.append('mem.w8(state.HL, {});'.format(rhs))
        else :
            src.append('{} = mem.r8(state.PC++);'.format(lhs))
        src.append(t(cycles))
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def uLD_r_IXYhl(ops, xname) :
    '''
    undocumented!
    LD r,s
    LD r,s
    LD r,s
    LD r,s
    where r and s is A,B,C,D,E,IXH,IXL,IYH,IYL
    T-states: 8(?)
    '''
    for r in r8 :
        for s in r8 :
            op = 0b01000000 | r.bits<<3 | s.bits
            rname = r.name
            if rname=='H' or rname=='L':
                rname = '{}{}'.format(xname,rname)
            sname = s.name
            if sname=='H' or sname=='L':
                sname = '{}{}'.format(xname,sname)
            src = [
                '// LD {},{}'.format(rname, sname),
                'state.{} = state.{};'.format(rname, sname),
                t(8)]
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def uLD_IXYhl_n(ops,xname) :
    '''
    undocumented!
    LD IXH,n
    LD IXL,n
    LD IYH,n
    LD IYL,n
    T-states: 11(?)
    '''
    for r in r8ixy :
        op = 0b00000110 | r.bits<<3
        src = [
            '// LD {}{},n'.format(xname, r.name),
            'state.{}{} = mem.r8(state.PC++);'.format(xname, r.name),
            t(11)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_A_iBC(ops) :
    '''
    LD A,(BC)
    T-states: 7
    '''
    src = [
        '// LD A,(BC)',
        'state.A = mem.r8(state.BC);',
        t(7)]
    return add_op(ops, 0x0A, src)

#-------------------------------------------------------------------------------
def LD_iBC_A(ops) :
    '''
    LD (BC),A
    T-states: 7
    ''' 
    src = [
        '// LD (BC),A',
        'mem.w8(state.BC, state.A);',
        t(7)]
    return add_op(ops, 0x02, src)

#-------------------------------------------------------------------------------
def LD_A_iDE(ops) :
    ''' 
    LD A,(DE)
    T-states: 7
    '''
    src = [
        '// LD A,(DE)',
        'state.A = mem.r8(state.DE);',
        t(7)]
    return add_op(ops, 0x1A, src)

#-------------------------------------------------------------------------------
def LD_iDE_A(ops) :
    '''
    LD (DE),A
    T-states: 7
    '''
    src = [
        '// LD (DE),A',
        'mem.w8(state.DE, state.A);',
        t(7)]
    return add_op(ops, 0x12, src)

#-------------------------------------------------------------------------------
def LD_A_inn(ops) :
    '''
    LD A,(nn)
    T-states: 13
    '''
    src = [
        '// LD A,(nn)',
        'state.A = mem.r8(mem.r16(state.PC));',
        'state.PC += 2;',
        t(13)]
    return add_op(ops, 0x3A, src)

#-------------------------------------------------------------------------------
def LD_inn_A(ops) :
    '''
    LD (nn),A
    T-states: 13
    '''
    src = [
        '// LD (nn),A',
        'mem.w8(mem.r16(state.PC), state.A);',
        'state.PC += 2;',
        t(13)]
    return add_op(ops, 0x32, src)

#-------------------------------------------------------------------------------
def LD_A_I(ops) :
    '''
    LD A,I
    T-states: 9
    '''
    src = [
        '// LD A,I',
        'state.A = state.I;',
        'state.F = sziff2(state.I,state.IFF2)|(state.F&CF);',
        t(9)]
    return add_op(ops, 0x57, src)

#-------------------------------------------------------------------------------
def LD_I_A(ops) :
    '''
    LD I,A
    T-states: 9
    '''
    src = [
        '// LD I,A',
        'state.I = state.A;',
        t(9)]
    return add_op(ops, 0x47, src)

#-------------------------------------------------------------------------------
def LD_A_R(ops) :
    '''
    LD A,R
    T-states: 9
    '''
    src = [
        '// LD A,R',
        'state.A = state.R;',
        'state.F = sziff2(state.R,state.IFF2)|(state.F&CF);',
        t(9)]
    return add_op(ops, 0x5F, src)

#-------------------------------------------------------------------------------
def LD_R_A(ops) :
    '''
    LD R,A
    T-states: 9
    '''
    src = [
        '// LD R,A',
        'state.R = state.A;',
        t(9)]
    return add_op(ops, 0x4F, src)

#-------------------------------------------------------------------------------
def LD_dd_nn(ops) :
    '''
    LD dd,nn
    T-states: 10
    '''
    for r in r16sp :
        op = 0b00000001 | r.bits<<4
        src = [
            '// LD {},nn'.format(r.name),
            'state.{} = mem.r16(state.PC);'.format(r.name),
            'state.PC += 2;',
            t(10)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_HL_inn(ops) :
    '''
    LD HL,(nn)
    T-states: 16
    '''
    src = [
        '// LD HL,(nn)',
        'state.HL = mem.r16(mem.r16(state.PC));',
        'state.PC += 2;',
        t(16)]
    return add_op(ops, 0x2A, src)

#-------------------------------------------------------------------------------
def LD_inn_HL(ops) :
    '''
    LD (nn),HL
    T-states: 16
    '''
    src = [
        '// LD (nn),HL',
        'mem.w16(mem.r16(state.PC), state.HL);',
        'state.PC += 2;',
        t(16)]
    return add_op(ops, 0x22, src)

#-------------------------------------------------------------------------------
def LD_dd_inn(ops) :
    '''
    LD dd,(nn)
    T-states: 20
    '''
    for r in r16sp :
        op = 0b01001011 | r.bits<<4
        src = [
            '// LD {},(nn)'.format(r.name),
            'state.{} = mem.r16(mem.r16(state.PC));'.format(r.name),
            'state.PC += 2;',
            t(20)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_inn_dd(ops) :
    '''
    LD (nn),dd
    T-states: 20
    '''
    for r in r16sp :
        op = 0b01000011 | r.bits<<4
        src = [
            '// LD (nn),{}'.format(r.name),
            'mem.w16(mem.r16(state.PC), state.{});'.format(r.name),
            'state.PC += 2;',
            t(20)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_SP_HL(ops) :
    '''
    LD SP,HL
    T-states: 6
    '''
    src = [
        '// LD SP,HL',
        'state.SP = state.HL;',
        t(6)]
    return add_op(ops, 0xF9, src)

#-------------------------------------------------------------------------------
def PUSH_qq(ops) :
    '''
    PUSH qq
    T-states: 11
    '''
    for r in r16af :
        op = 0b11000101 | r.bits<<4
        src = [
            '// PUSH {}'.format(r.name),
            'state.SP -= 2;',
            'mem.w16(state.SP, state.{});'.format(r.name),
            t(11)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def POP_qq(ops) :
    '''
    POP qq
    T-states: 10
    '''
    for r in r16af :
        op = 0b11000001 | r.bits<<4
        src = [
            '// POP {}'.format(r.name),
            'state.{} = mem.r16(state.SP);'.format(r.name),
            'state.SP += 2;',
            t(10)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def PUSH_IXY(ops, xname) :
    '''
    PUSH [IX|IY]
    T-states: 15
    '''
    src = [
        '// PUSH {}'.format(xname),
        'state.SP -= 2;',
        'mem.w16(state.SP, state.{});'.format(xname),
        t(15)]
    return add_op(ops, 0xE5, src)

#-------------------------------------------------------------------------------
def POP_IXY(ops, xname) :
    '''
    POP [IX|IY]
    T-states: 14
    '''
    src = [
        '// POP {}'.format(xname),
        'state.{} = mem.r16(state.SP);'.format(xname),
        'state.SP += 2;',
        t(14)]
    return add_op(ops, 0xE1, src)

#-------------------------------------------------------------------------------
def LD_r_iIXY_d(ops, xname) :
    ''' 
    LD r,([IX|IY]+d)
    T-states: 19
    '''
    for r in r8 :
        op = 0b01000110 | r.bits<<3
        src = [
            '// LD {},({}+d)'.format(r.name, xname),
            'd = mem.rs8(state.PC++);',
            'state.{} = mem.r8(state.{} + d);'.format(r.name, xname),
            t(19)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_iIXY_d_n(ops, xname) :
    '''
    LD ([IX|IY]+d),n
    T-states: 19
    '''
    src = [
        '// LD ({}+d),n'.format(xname),
        'd = mem.rs8(state.PC++);',
        'mem.w8(state.{} + d, mem.r8(state.PC++));'.format(xname),
        t(19)]
    return add_op(ops, 0x36, src)

#-------------------------------------------------------------------------------
def LD_iIXY_d_r(ops, xname) :
    '''
    LD ([IX|IY]+d),r
    T-states: 19
    '''
    for r in r8 :
        op = 0b01110000 | r.bits
        src = [
            '// LD ({}+d),{}'.format(xname, r.name),
            'd = mem.rs8(state.PC++);',
            'mem.w8(state.{} + d, state.{});'.format(xname, r.name),
            t(19)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def LD_IXY_nn(ops, xname) :
    '''
    LD [IX|IY],nn
    T-states: 14
    '''
    src = [
        '// LD {},nn'.format(xname),
        'state.{} = mem.r16(state.PC);'.format(xname),
        'state.PC += 2;',
        t(14)]
    return add_op(ops, 0x21, src)

#-------------------------------------------------------------------------------
def LD_IXY_inn(ops, xname) :
    '''
    LD [IX|IY],(nn)
    T-states: 20
    '''
    src = [
        '// LD {},(nn)'.format(xname),
        'state.{} = mem.r16(mem.r16(state.PC));'.format(xname),
        'state.PC += 2;',
        t(20)]
    return add_op(ops, 0x2A, src)

#-------------------------------------------------------------------------------
def LD_inn_IXY(ops, xname) :
    '''
    LD (nn),[IX|IY]
    T-states: 20
    '''
    src = [
        '// LD (nn),{}'.format(xname),
        'mem.w16(mem.r16(state.PC), state.{});'.format(xname),
        'state.PC += 2;',
        t(20)]
    return add_op(ops, 0x22, src)

#-------------------------------------------------------------------------------
def LD_SP_IXY(ops, xname) :
    '''
    LD SP,[IX|IY]
    T-states: 10
    '''
    src = [
        '// LD SP,{}'.format(xname),
        'state.SP = state.{};'.format(xname),
        t(10)]
    return add_op(ops, 0xF9, src)

#-------------------------------------------------------------------------------
def EX_DE_HL(ops) :
    '''
    EX DE,HL
    T-states: 4
    '''
    src = [
        '// EX DE,HL',
        'swap16(state.DE, state.HL);',
        t(4)]
    return add_op(ops, 0xEB, src)

#-------------------------------------------------------------------------------
def EX_AF_AFx(ops) :
    '''
    EX AF,AF'
    T-states: 4
    '''
    src = [
        "// EX AF,AF'",
        'swap16(state.AF, state.AF_);',
        t(4)]
    return add_op(ops, 0x08, src)

#-------------------------------------------------------------------------------
def EXX(ops) :
    '''
    EXX
    T-states: 4
    '''
    src = [
        '// EXX',
        'swap16(state.BC, state.BC_);',
        'swap16(state.DE, state.DE_);',
        'swap16(state.HL, state.HL_);',
        t(4)]
    return add_op(ops, 0xD9, src)

#-------------------------------------------------------------------------------
def EX_iSP_HLIXY(ops, xname) :
    '''
    EX (SP),[HL|IX|IY]
    T-states: 19 or 23
    '''
    src = [
        '// EX (SP),{}'.format(xname),
        'u16tmp = mem.r16(state.SP);',
        'mem.w16(state.SP, state.{});'.format(xname),
        'state.{} = u16tmp;'.format(xname),
        t(19) if xname == 'HL' else t(23)]
    return add_op(ops, 0xE3, src)

#-------------------------------------------------------------------------------
def ALU_r(ops) :
    alu_ops = [
        ['ADD', 'add8', 0b10000000],
        ['ADC', 'adc8', 0b10001000],
        ['SUB', 'sub8', 0b10010000],
        ['SBC', 'sbc8', 0b10011000],
        ['AND', 'and8', 0b10100000],
        ['XOR', 'xor8', 0b10101000], 
        ['OR',  'or8',  0b10110000],
        ['CP',  'cp8',  0b10111000]
    ]
    for alu in alu_ops : 
        for r in r8ihl :
            op = alu[2] | r.bits
            if r.mem :
                rhs = 'mem.r8(state.HL)'
                cycles = 7
            else :
                rhs = 'state.{}'.format(r.name)
                cycles = 4
            src = [
                '// {} {}'.format(alu[0], r.name),
                '{}({});'.format(alu[1], rhs),
                t(cycles)]
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def ALU_n(ops) :
    alu_ops = [
        ['ADD', 'add8', 0xC6],
        ['ADC', 'adc8', 0xCE],
        ['SUB', 'sub8', 0xD6],
        ['SBC', 'sbc8', 0xDE],
        ['AND', 'and8', 0xE6],
        ['XOR', 'xor8', 0xEE], 
        ['OR',  'or8',  0xF6],
        ['CP',  'cp8',  0xFE]
    ]
    for alu in alu_ops : 
        op = alu[2]
        src = [
            '// {} n'.format(alu[0]),
            '{}(mem.r8(state.PC++));'.format(alu[1]),
            t(7)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def uALU_IXYhl(ops, xname) :
    '''
    undocumented!
    '''
    # note: these are the same as in ALU_r
    alu_ops = [
        ['ADD', 'add8', 0b10000000],
        ['ADC', 'adc8', 0b10001000],
        ['SUB', 'sub8', 0b10010000],
        ['SBC', 'sbc8', 0b10011000],
        ['AND', 'and8', 0b10100000],
        ['XOR', 'xor8', 0b10101000], 
        ['OR',  'or8',  0b10110000],
        ['CP',  'cp8',  0b10111000]
    ]
    for alu in alu_ops :
        for r in r8ixy :
            op = alu[2] | r.bits
            src = [
                '// {} {}{}'.format(alu[0], xname, r.name),
                '{}(state.{}{});'.format(alu[1], xname, r.name),
                t(8)]
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def ADD_iIXY_d(ops, xname) :
    src = [
        '// ADD ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'add8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0x86, src)

#-------------------------------------------------------------------------------
def ADC_iIXY_d(ops, xname) :
    src = [
        '// ADC ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'adc8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0x8E, src)

#-------------------------------------------------------------------------------
def SUB_iIXY_d(ops, xname) :
    '''
    SUB ([IX|IY]+d)
    T-states: 19
    '''
    src = [
        '// SUB ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'sub8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0x96, src)

#-------------------------------------------------------------------------------
def CP_iIXY_d(ops, xname) :
    '''
    CP ([IX|IY]+d)
    T-states: 19
    '''
    src = [
        '// CP ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'cp8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0xBE, src)

#-------------------------------------------------------------------------------
def SBC_A_iIXY_d(ops, xname) :
    '''
    SBC A,([IX|IY]+d)
    T-states: 19
    '''
    src = [
        '// SBC A,({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'sbc8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0x9E, src)

#-------------------------------------------------------------------------------
def AND_iIXY_d(ops, xname) :
    '''
    AND ([IX|IY]+d)
    T-states: 19
    '''
    src = [
        '// AND ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'and8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0xA6, src)

#-------------------------------------------------------------------------------
def OR_iIXY_d(ops, xname) :
    '''
    OR ([IX|IY]+d)
    T-states: 19
    '''
    src = [
        '// OR ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'or8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0xB6, src)

#-------------------------------------------------------------------------------
def XOR_iIXY_d(ops, xname) :
    '''
    XOR ([IX|IY]+d)
    T-states: 19
    '''
    src = [
        '// XOR ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'xor8(mem.r8(state.{} + d));'.format(xname),
        t(19)]
    return add_op(ops, 0xAE, src)

#-------------------------------------------------------------------------------
def INC_r(ops) :
    '''
    INC r
    T-states: 4
    '''
    for r in r8 :
        op = 0b00000100 | r.bits<<3
        src = [
            '// INC {}'.format(r.name),
            'state.{} = inc8(state.{});'.format(r.name, r.name),
            t(4)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def uINC_IXYhl(ops, xname) :
    '''
    undocumented!
    INC IXH
    INC IXL
    INC IYH
    INC IYL
    T-states: 8(?)
    '''
    for r in r8ixy:
        op = 0b00000100 | r.bits<<3
        src = [
            '// INC {}{}'.format(xname, r.name),
            'state.{}{} = inc8(state.{}{});'.format(xname, r.name, xname, r.name),
            t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def DEC_r(ops) :
    '''
    DEC r
    T-states: 4
    '''
    for r in r8 :
        op = 0b00000101 | r.bits<<3
        src = [
            '// DEC {}'.format(r.name),
            'state.{} = dec8(state.{});'.format(r.name, r.name),
            t(4)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def uDEC_IXYhl(ops, xname) :
    '''
    undocumented!
    DEC IXH
    DEC IXL
    DEC IYH
    DEC IYL
    T-states: 8(?)
    '''
    for r in r8ixy:
        op = 0b00000101 | r.bits<<3
        src = [
            '// DEC {}{}'.format(xname, r.name),
            'state.{}{} = dec8(state.{}{});'.format(xname, r.name, xname, r.name),
            t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def INC_iHL(ops) :
    '''
    INC (HL)
    T-states: 11
    '''
    src = [
        '// INC (HL)',
        'mem.w8(state.HL, inc8(mem.r8(state.HL)));',
        t(11)]
    return add_op(ops, 0x34, src)

#-------------------------------------------------------------------------------
def DEC_iHL(ops) :
    '''
    DEC (HL)
    T-states: 11
    '''
    src = [
        '// DEC (HL)',
        'mem.w8(state.HL, dec8(mem.r8(state.HL)));',
        t(11)]
    return add_op(ops, 0x35, src)

#-------------------------------------------------------------------------------
def INC_iIXY_d(ops, xname) :
    '''
    INC ([IX|IY]+d)
    T-states: 23
    '''
    src = [
        '// INC ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'mem.w8(state.{}+d, inc8(mem.r8(state.{}+d)));'.format(xname, xname),
        t(23)]
    return add_op(ops, 0x34, src)

#-------------------------------------------------------------------------------
def DEC_iIXY_d(ops, xname) :
    '''
    DEC ([IX|IY]+d)
    T-states: 23
    '''
    src = [
        '// DEC ({}+d)'.format(xname),
        'd = mem.rs8(state.PC++);',
        'mem.w8(state.{}+d, dec8(mem.r8(state.{}+d)));'.format(xname, xname),
        t(23)]
    return add_op(ops, 0x35, src)

#-------------------------------------------------------------------------------
def INC_ss(ops) :
    '''
    INC ss
    T-states: 6
    '''
    for r in r16sp :
        op = 0b00000011 | r.bits<<4
        src = [
            '// INC {}'.format(r.name),
            'state.{}++;'.format(r.name),
            t(6)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def DEC_ss(ops) :
    '''
    DEC ss
    T-states: 6
    '''
    for r in r16sp :
        op = 0b00001011 | r.bits<<4
        src = [
            '// DEC {}'.format(r.name),
            'state.{}--;'.format(r.name),
            t(6)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def INC_IXY(ops, xname) :
    '''
    INC [IX|IY]
    T-states: 10
    '''
    src = [
        '// INC {}'.format(xname),
        'state.{}++;'.format(xname),
        t(10)]
    return add_op(ops, 0x23, src)

#-------------------------------------------------------------------------------
def DEC_IXY(ops, xname) :
    '''
    DEC [IX|IY]
    T-states: 10
    '''
    src = [
        '// DEC {}'.format(xname),
        'state.{}--;'.format(xname),
        t(10)]
    return add_op(ops, 0x2B, src)

#-------------------------------------------------------------------------------
def RLCA(ops) :
    '''
    RLCA
    T-state: 4
    '''
    src = ['// RLCA', 'state.A = rlc8(state.A, false);', t(4)]
    return add_op(ops, 0x07, src)

#-------------------------------------------------------------------------------
def RLA(ops) :
    '''
    RLA
    T-states: 4
    '''
    src = ['// RLA', 'state.A = rl8(state.A, false);', t(4)]
    return add_op(ops, 0x17, src)

#-------------------------------------------------------------------------------
def RRCA(ops) :
    '''
    RRCA
    T-states: 4
    '''
    src = ['// RRCA', 'state.A = rrc8(state.A, false);', t(4)]
    return add_op(ops, 0x0F, src)

#-------------------------------------------------------------------------------
def RRA(ops) :
    '''
    RRA
    T-states: 4
    '''
    src = ['// RRA', 'state.A = rr8(state.A, false);', t(4)]
    return add_op(ops, 0x1F, src)

#-------------------------------------------------------------------------------
def RLC_r(ops) :
    '''
    RLC r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00000000 | r.bits
        src = ['// RLC {}'.format(r.name),
            'state.{} = rlc8(state.{}, true);'.format(r.name, r.name),
            t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RRC_r(ops) :
    '''
    RRC r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00001000 | r.bits
        src = ['// RRC {}'.format(r.name),
            'state.{} = rrc8(state.{}, true);'.format(r.name, r.name),
            t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RLC_iHL(ops) :
    '''
    RLC (HL)
    T-states: 15
    '''
    src = ['// RLC (HL)',
        'mem.w8(state.HL, rlc8(mem.r8(state.HL), true));',
        t(15)]
    return add_op(ops, 0x06, src)

#-------------------------------------------------------------------------------
def RRC_iHL(ops) :
    '''
    RRC (HL)
    T-states: 15
    '''
    src = ['// RRC (HL)',
        'mem.w8(state.HL, rrc8(mem.r8(state.HL), true));',
        t(15)]
    return add_op(ops, 0x0E, src)

#-------------------------------------------------------------------------------
def RL_r(ops) :
    '''
    RL r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00010000 | r.bits
        src = ['// RL {}'.format(r.name),
            'state.{} = rl8(state.{}, true);'.format(r.name, r.name),
            t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RR_r(ops) :
    '''
    RR r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00011000 | r.bits
        src = ['// RR {}'.format(r.name),
            'state.{} = rr8(state.{}, true);'.format(r.name, r.name),
            t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RL_iHL(ops) :
    '''
    RL (HL)
    T-states: 15
    '''
    src = ['// RL (HL)',
        'mem.w8(state.HL, rl8(mem.r8(state.HL), true));',
        t(15)]
    return add_op(ops, 0x16, src)

#-------------------------------------------------------------------------------
def RR_iHL(ops) :
    '''
    RR (HL)
    T-states: 15
    '''
    src = ['// RR (HL)',
        'mem.w8(state.HL, rr8(mem.r8(state.HL), true));',
        t(15)]
    return add_op(ops, 0x1E, src)

#-------------------------------------------------------------------------------
def SLA_r(ops) :
    '''
    SLA r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00100000 | r.bits
        src = ['// SLA {}'.format(r.name),
        'state.{} = sla8(state.{});'.format(r.name, r.name),
        t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def uSLL_r(ops) :
    '''
    undocumented!
    SLL r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00110000 | r.bits
        src = ['// SLL {}'.format(r.name),
               'state.{} = sll8(state.{});'.format(r.name, r.name),
               t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def SRA_r(ops) :
    '''
    SRA r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00101000 | r.bits
        src = ['// SRA {}'.format(r.name),
        'state.{} = sra8(state.{});'.format(r.name, r.name),
        t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def SRL_r(ops) :
    '''
    SRL r
    T-states: 8
    '''
    for r in r8 :
        op = 0b00111000 | r.bits
        src = ['// SRL {}'.format(r.name),
        'state.{} = srl8(state.{});'.format(r.name, r.name),
        t(8)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def SLA_iHL(ops) :
    '''
    SLA (HL)
    T-states: 15
    '''
    src = ['// SLA (HL)', 'mem.w8(state.HL, sla8(mem.r8(state.HL)));', t(15)]
    return add_op(ops, 0x26, src)

#-------------------------------------------------------------------------------
def uSLL_iHL(ops) :
    '''
    undocumented!
    SLL (HL)
    T-states: 15
    '''
    src = ['// SLL (HL)', 'mem.w8(state.HL, sll8(mem.r8(state.HL)));', t(15)]
    return add_op(ops, 0x36, src)

#-------------------------------------------------------------------------------
def SRA_iHL(ops) :
    '''
    SRA (HL)
    T-states: 15
    '''
    src = ['// SRA (HL)',
        'mem.w8(state.HL, sra8(mem.r8(state.HL)));',
        t(15)]
    return add_op(ops, 0x2E, src)

#-------------------------------------------------------------------------------
def SRL_iHL(ops) :
    '''
    SRL (HL)
    T-states: 15
    '''
    op = 0b00111110
    src = ['// SRL (HL)',
        'mem.w8(state.HL, srl8(mem.r8(state.HL)));',
        t(15)]
    return add_op(ops, 0x3E, src)

#-------------------------------------------------------------------------------
def RLD(ops) :
    '''
    RLD
    T-states: 18
    '''
    src = ['// RLD', 'rld();', t(18)]
    return add_op(ops, 0x6F, src)

#-------------------------------------------------------------------------------
def RRD(ops) :
    '''
    RRD
    T-states: 18
    '''
    src = ['// RRD', 'rrd();', t(18)]
    return add_op(ops, 0x67, src)

#-------------------------------------------------------------------------------
def BIT_b_r(ops) :
    '''
    BIT b,r
    T-states: 8
    '''
    for b in range(0,8) :
        for r in r8 :
            op = 0b01000000 | b<<3 | r.bits
            src = ['// BIT {},{}'.format(b, r.name),
                'bit(state.{}, (1<<{}));'.format(r.name, b),
                t(8)]
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def BIT_b_iHL(ops) :
    '''
    BIT b,(HL)
    T-states: 12
    '''
    for b in range(0,8) :
        op = 0b01000110 | b<<3
        src = ['// BIT {},(HL)'.format(b),
            'bit(mem.r8(state.HL), (1<<{}));'.format(b),
            t(12)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def SET_b_r(ops) :
    '''
    SET b,r
    T-states: 8
    '''
    for b in range(0,8) :
        for r in r8 :
            op = 0b11000000 | b<<3 | r.bits
            src = ['// SET {},{}'.format(b, r.name),
                'state.{} |= (1<<{});'.format(r.name, b),
                t(8)]
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def SET_b_iHL(ops) :
    '''
    SET b,(HL)
    T-states: 15
    '''
    for b in range(0, 8) :
        op = 0b11000110 | b<<3
        src = ['// SET {},(HL)'.format(b),
            'mem.w8(state.HL, mem.r8(state.HL)|(1<<{}));'.format(b),
            t(15)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RES_b_r(ops) :
    '''
    RES b,r
    T-states: 8
    '''
    for b in range(0, 8) :
        for r in r8 :
            op = 0b10000000 | b<<3 | r.bits
            src = ['// RES {},{}'.format(b, r.name),
                'state.{} &= ~(1<<{});'.format(r.name, b),
                t(8)]
            ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RES_b_iHL(ops) :
    '''
    RES b,(HL)
    T-states: 15
    '''
    for b in range(0, 8) :
        op = 0b10000110 | b<<3
        src = ['// RES {},(HL)'.format(b),
            'mem.w8(state.HL, mem.r8(state.HL)&~(1<<{}));'.format(b),
            t(15)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def DD_FD_CB(ops, lead_byte) :
    '''
    RLC, RRC, RL, RR, SLA, SRA, SRL, BIT, SET, RES for ([IX|IY]+d)
    T-states: 23, 20
    '''
    src = [
        '// RLC ([IX|IY]+d)',
        '// RRC ([IX|IY]+d)',
        '// RL ([IX|IY]+d)',
        '// RR ([IX|IY]+d)',
        '// SLA ([IX|IY]+d)',
        '// SRA ([IX|IY]+d)',
        '// SRL ([IX|IY]+d)',
        '// BIT b,([IX|IY]+d',
        '// SET b,([IX|IY]+d',
        '// RES b,([IX|IY]+d',
        'return dd_fd_cb({});'.format(hex(lead_byte))]
    return add_op(ops, 0xCB, src)

#-------------------------------------------------------------------------------
def LDI(ops) :
    '''
    LDI
    T-states: 16
    '''
    src = ['// LDI', 'ldi();', t(16)]
    return add_op(ops, 0xA0, src)

#-------------------------------------------------------------------------------
def LDIR(ops) :
    '''
    LDIR
    T-states: 21/16
    '''
    src = ['// LDIR', 'return ldir();']
    return add_op(ops, 0xB0, src)

#-------------------------------------------------------------------------------
def LDD(ops) :
    '''
    LDD
    T-states: 16
    '''
    src = ['// LDD', 'ldd();', t(16)]
    return add_op(ops, 0xA8, src)

#-------------------------------------------------------------------------------
def LDDR(ops) :
    '''
    LDDR
    T-states: 21/16
    '''
    src = ['// LDDR', 'return lddr();']
    return add_op(ops, 0xB8, src)

#-------------------------------------------------------------------------------
def CPI(ops) :
    '''
    CPI
    T-states: 16
    '''
    src = ['// CPI', 'cpi();', t(16)]
    return add_op(ops, 0xA1, src)

#-------------------------------------------------------------------------------
def CPIR(ops) :
    '''
    CPIR
    T-states: 21/16
    '''
    src = ['// CPIR', 'return cpir();']
    return add_op(ops, 0xB1, src)

#-------------------------------------------------------------------------------
def CPD(ops) :
    '''
    CPD
    T-states: 16
    '''
    src = ['// CPD', 'cpd();', t(16)]
    return add_op(ops, 0xA9, src)

#-------------------------------------------------------------------------------
def CPDR(ops) :
    '''
    CPDR
    T-states: 21/16
    '''
    src = ['// CPDR', 'return cpdr();']
    return add_op(ops, 0xB9, src)

#-------------------------------------------------------------------------------
def DAA(ops) :
    '''
    DAA
    T-states: 4
    '''
    src = ['// DAA', 'daa();', t(4)]
    return add_op(ops, 0x27, src)

#-------------------------------------------------------------------------------
def CPL(ops) :
    '''
    CPL
    T-states: 4
    '''
    src = ['// CPL',
        'state.A ^= 0xFF;',
        'state.F = (state.F&(SF|ZF|PF|CF)) | HF | NF | (state.A&(YF|XF));',
        t(4)]
    return add_op(ops, 0x2F, src)

#-------------------------------------------------------------------------------
def NEG(ops) :
    '''
    NEG
    T-states: 8
    '''
    src = ['// NEG', 'neg8();', t(8)]
    return add_op(ops, 0x44, src)

#-------------------------------------------------------------------------------
def CCF(ops) :
    '''
    CCF
    T-states: 4
    '''
    src = ['// CCF',
        'state.F = ((state.F&(SF|ZF|YF|XF|PF|CF))|((state.F&CF)<<4)|(state.A&(YF|XF)))^CF;',
        t(4)]
    return add_op(ops, 0x3F, src)

#-------------------------------------------------------------------------------
def SCF(ops) :
    '''
    SCF
    T-states: 4
    '''
    src = ['// SCF',
        'state.F = (state.F&(SF|ZF|YF|XF|PF))|CF|(state.A&(YF|XF));',
        t(4)]
    return add_op(ops, 0x37, src)

#-------------------------------------------------------------------------------
def JP_nn(ops) :
    '''
    JP nn
    T-states: 10
    '''
    src = ['// JP nn',
        'state.PC = mem.r16(state.PC);',
        t(10)]
    return add_op(ops, 0xC3, src)

#-------------------------------------------------------------------------------
def JP_cc_nn(ops) :
    '''
    JP cc,nn
    T-states: 10
    '''
    for c in cc :
        op = 0b11000010 | c.bits<<3
        src = [
            '// JP {},nn'.format(c.name),
            'state.PC = {} ? mem.r16(state.PC) : state.PC+2;'.format(c.test),
            t(10)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def JR_e(ops) :
    '''
    JR e
    T-states: 12
    '''
    src = ['// JR e', 'state.PC += mem.rs8(state.PC++);', t(12)]
    return add_op(ops, 0x18, src)

#-------------------------------------------------------------------------------
def JR_cc_e(ops) :
    '''
    JR C,e
    JR NC,e
    JR Z,e
    JR NZ,e
    T-state: 12/7
    '''
    CC = namedtuple('CC', 'op name test')
    cc = [
        CC(0x38, 'C',  'state.F & CF'),
        CC(0x30, 'NC', '!(state.F & CF)'),
        CC(0x28, 'Z',  'state.F & ZF'),
        CC(0x20, 'NZ', '!(state.F & ZF)')
    ]
    for c in cc :
        src = ['// JR {},e'.format(c.name),
            'if ({}) {{'.format(c.test),
            '    state.PC += mem.rs8(state.PC++);',
            '    return 12;',
            '}',
            'else {',
            '    state.PC++;',
            '    return 7;',
            '}']
        ops = add_op(ops, c.op, src)
    return ops

#-------------------------------------------------------------------------------
def JP_iHL(ops) :
    '''
    JP (HL)
    T-states: 4
    '''
    src = ['// JP (HL)', 'state.PC = state.HL;', t(4)]
    return add_op(ops, 0xE9, src)

#-------------------------------------------------------------------------------
def JP_iIXY(ops, xname) :
    '''
    JP ([IX|IY])
    T-states: 8
    '''
    src = ['// JP ({})'.format(xname), 'state.PC = state.{};'.format(xname), t(8)]
    return add_op(ops, 0xE9, src)

#-------------------------------------------------------------------------------
def DJNZ_e(ops) :
    '''
    DJNZ e
    T-states: 13/8
    '''
    src = [
        '// DJNZ e',
        'if (--state.B > 0) {',
        '    state.PC += mem.rs8(state.PC++);',
        '    return 13;',
        '}',
        'else {',
        '    state.PC++;',
        '    return 8;',
        '}']
    return add_op(ops, 0x10, src)

#-------------------------------------------------------------------------------
def CALL_nn(ops) :
    '''
    CALL nn
    T-states: 17
    '''
    src = [
        '// CALL nn',
        'state.SP -= 2;',
        'mem.w16(state.SP, state.PC+2);',
        'state.PC = mem.r16(state.PC);',
        t(17)]
    ops = add_op(ops, 0xCD, src)
    return ops

#-------------------------------------------------------------------------------
def CALL_cc_nn(ops) :
    '''
    CALL cc,nn
    T-states: 17,10
    '''
    for c in cc :
        op = 0b11000100 | c.bits<<3
        src = ['// CALL {},nn'.format(c.name),
            'if ({}) {{'.format(c.test),
            '    state.SP -= 2;',
            '    mem.w16(state.SP, state.PC+2);',
            '    state.PC = mem.r16(state.PC);',
            '    return 17;',
            '}',
            'else {',
            '    state.PC += 2;',
            '    return 10;',
            '}']
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RET(ops) :
    '''
    RET
    T-states: 10
    '''
    src = ['// RET',
        'state.PC = mem.r16(state.SP);',
        'state.SP += 2;',
        t(10)]
    return add_op(ops, 0xC9, src)

#-------------------------------------------------------------------------------
def RET_cc(ops) :
    '''
    RET cc
    T-states: 11/5
    '''
    for c in cc :
        op = 0b11000000 | c.bits<<3
        src = ['// RET {}'.format(c.name),
            'if ({}) {{'.format(c.test),
            '    state.PC = mem.r16(state.SP);',
            '    state.SP += 2;',
            '    return 11;',
            '}',
            'else {',
            '    return 5;',
            '}']
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def RETI(ops) :
    '''
    RETI
    T-states: 14
    '''
    src = ['// RETI', 'reti();', t(15)]
    return add_op(ops, 0x4D, src);

#-------------------------------------------------------------------------------
def IN_A_in(ops) :
    '''
    IN A,(n)
    T-states: 11
    '''
    src = ['// IN A,(n)', 'state.A = in((state.A<<8)|(mem.r8(state.PC++)));', t(11)]
    return add_op(ops, 0xDB, src)

#-------------------------------------------------------------------------------
def IN_r_iC(ops) :
    '''
    IN r,(C)
    T-states: 12
    '''
    for r in r8:
        op = 0b01000000 | r.bits<<3
        src = ['// IN {},(C)'.format(r.name),
            'state.{} = in(state.BC);'.format(r.name),
            'state.F = szp[state.{}]|(state.F&CF);'.format(r.name),
            t(12)]
        add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def INI(ops) :
    '''
    INI
    T-states: 16
    '''
    src = ['// INI', 'ini();', t(16)]
    return add_op(ops, 0xA2, src)

#-------------------------------------------------------------------------------
def INIR(ops) :
    '''
    INIR
    T-states: 21/16
    '''
    src = ['// INIR', 'return inir();']
    return add_op(ops, 0xB2, src)

#-------------------------------------------------------------------------------
def IND(ops) :
    '''
    IND
    T-states: 16
    '''
    src = ['// IND', 'ind();', t(16)]
    return add_op(ops, 0xAA, src)

#-------------------------------------------------------------------------------
def INDR(ops) :
    '''
    INDR
    T-states: 21/16
    '''
    src = ['// INDR', 'return indr();']
    return add_op(ops, 0xBA, src)

#-------------------------------------------------------------------------------
def OUT_in_A(ops) :
    '''
    OUT (n),A
    T-states: 11
    '''
    src = ['// OUT (n),A', 'out((state.A<<8)|mem.r8(state.PC++),state.A);', t(11)]
    return add_op(ops, 0xD3, src)

#-------------------------------------------------------------------------------
def OUT_iC_r(ops) :
    '''
    OUT (C),r
    T-states: 12
    '''
    for r in r8 :
        op = 0b01000001 | r.bits<<3
        src = ['// OUT (C),{}'.format(r.name),
            'out(state.BC, state.{});'.format(r.name),
            t(12)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def OUTI(ops) :
    '''
    OUTI
    T-states: 16
    '''
    src = ['// OUTI', 'outi();', t(16)]
    return add_op(ops, 0xA3, src)

#-------------------------------------------------------------------------------
def OTIR(ops) :
    '''
    OTIR
    T-state: 21/16
    '''
    src = ['// OTIR', 'return otir();']
    return add_op(ops, 0xB3, src)

#-------------------------------------------------------------------------------
def OUTD(ops) :
    '''
    OUTD
    T-states: 16
    '''
    src = ['// OUTD', 'outd();', t(16)]
    return add_op(ops, 0xAB, src)

#-------------------------------------------------------------------------------
def OTDR(ops) :
    '''
    OTDR
    T-states: 21/16
    '''
    src = ['// OTDR', 'return otdr();']
    return add_op(ops, 0xBB, src)

#-------------------------------------------------------------------------------
def ADD_HL_ss(ops) :
    '''
    ADD HL,ss
    T-states: 11
    '''
    for r in r16sp :
        op = 0b00001001 | r.bits<<4
        src = ['// ADD HL,{}'.format(r.name),
            'state.HL = add16(state.HL, state.{});'.format(r.name),
            t(11)]
        ops = add_op(ops, op, src)
    return ops
   
#-------------------------------------------------------------------------------
def ADC_HL_ss(ops) :
    '''
    ADC HL,ss
    T-states: 15
    '''
    for r in r16sp :
        op = 0b01001010 | r.bits<<4
        src = ['// ADC HL,{}'.format(r.name),
            'state.HL = adc16(state.HL, state.{});'.format(r.name),
            t(15)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def SBC_HL_ss(ops) :
    '''
    SBC HL,ss
    T-states: 15
    '''
    for r in r16sp :
        op = 0b01000010 | r.bits<<4
        src = ['// SBC HL,{}'.format(r.name),
            'state.HL = sbc16(state.HL, state.{});'.format(r.name),
            t(15)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def ADD_IXY_pp(ops, xname) :
    '''
    ADD [IX|IY],pp
    T-states: 15
    '''
    for r in r16ixy[xname] :
        op = 0b00001001 | r.bits<<4
        src = ['// ADD {},{}'.format(xname, r.name),
            'state.{} = add16(state.{}, state.{});'.format(xname, xname, r.name),
            t(15)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def gen_opcodes() :
    '''
    Generates the single-byte opcode table.
    '''
    ops = {}
    ops = NOP_HALT_DI_EI(ops)
    ops = LD_r_s(ops)
    ops = LD_r_n(ops)
    ops = LD_A_iBC(ops)
    ops = LD_iBC_A(ops)
    ops = LD_A_iDE(ops)
    ops = LD_iDE_A(ops)
    ops = LD_A_inn(ops)
    ops = LD_inn_A(ops)
    ops = LD_dd_nn(ops)
    ops = LD_HL_inn(ops)
    ops = LD_inn_HL(ops)
    ops = LD_SP_HL(ops)
    ops = PUSH_qq(ops)
    ops = POP_qq(ops)
    ops = EX_DE_HL(ops)
    ops = EX_AF_AFx(ops)
    ops = EXX(ops)
    ops = EX_iSP_HLIXY(ops, 'HL')
    ops = ALU_r(ops)
    ops = ALU_n(ops)
    ops = INC_r(ops)
    ops = INC_iHL(ops)
    ops = DEC_r(ops)
    ops = DEC_iHL(ops)
    ops = INC_ss(ops)
    ops = DEC_ss(ops)
    ops = RLCA(ops)
    ops = RLA(ops)
    ops = RRCA(ops)
    ops = RRA(ops)
    ops = DAA(ops)
    ops = CPL(ops)
    ops = CCF(ops)
    ops = SCF(ops)
    ops = JP_nn(ops)
    ops = JP_cc_nn(ops)
    ops = JP_iHL(ops)
    ops = JR_e(ops)
    ops = JR_cc_e(ops)
    ops = DJNZ_e(ops)
    ops = CALL_nn(ops)
    ops = CALL_cc_nn(ops)
    ops = RET(ops)
    ops = RET_cc(ops)
    ops = IN_A_in(ops)
    ops = OUT_in_A(ops)
    ops = ADD_HL_ss(ops)
    ops[0xCB] = []
    ops[0xDD] = []
    ops[0xFD] = []
    ops[0xED] = []

    return ops

#-------------------------------------------------------------------------------
def gen_cb_opcodes() :
    '''
    Generate the CB-lead-byte opcode table
    '''
    cb_ops = {}
    cb_ops = RLC_r(cb_ops)
    cb_ops = RLC_iHL(cb_ops)
    cb_ops = RRC_r(cb_ops)
    cb_ops = RRC_iHL(cb_ops)
    cb_ops = RL_r(cb_ops)
    cb_ops = RL_iHL(cb_ops)
    cb_ops = RR_r(cb_ops)
    cb_ops = RR_iHL(cb_ops)
    cb_ops = SLA_r(cb_ops)
    cb_ops = uSLL_r(cb_ops)
    cb_ops = SLA_iHL(cb_ops)
    cb_ops = uSLL_iHL(cb_ops)
    cb_ops = SRA_r(cb_ops)
    cb_ops = SRA_iHL(cb_ops)
    cb_ops = SRL_r(cb_ops)
    cb_ops = SRL_iHL(cb_ops)
    cb_ops = BIT_b_r(cb_ops)
    cb_ops = BIT_b_iHL(cb_ops)
    cb_ops = SET_b_r(cb_ops)
    cb_ops = SET_b_iHL(cb_ops)
    cb_ops = RES_b_r(cb_ops)
    cb_ops = RES_b_iHL(cb_ops)
    return cb_ops

#-------------------------------------------------------------------------------
def gen_dd_opcodes() :
    '''
    Generate the DD-lead-byte opcode table.
    '''
    dd_ops = {}
    dd_ops = LD_r_iIXY_d(dd_ops, 'IX')
    dd_ops = LD_iIXY_d_r(dd_ops, 'IX')
    dd_ops = LD_iIXY_d_n(dd_ops, 'IX')
    dd_ops = LD_IXY_nn(dd_ops, 'IX')
    dd_ops = LD_IXY_inn(dd_ops, 'IX')
    dd_ops = LD_inn_IXY(dd_ops, 'IX')
    dd_ops = LD_SP_IXY(dd_ops, 'IX')
    dd_ops = PUSH_IXY(dd_ops, 'IX')
    dd_ops = POP_IXY(dd_ops, 'IX')
    dd_ops = EX_iSP_HLIXY(dd_ops, 'IX')
    dd_ops = ADD_iIXY_d(dd_ops, 'IX')
    dd_ops = ADC_iIXY_d(dd_ops, 'IX')
    dd_ops = SUB_iIXY_d(dd_ops, 'IX')
    dd_ops = CP_iIXY_d(dd_ops, 'IX') 
    dd_ops = SBC_A_iIXY_d(dd_ops, 'IX')
    dd_ops = AND_iIXY_d(dd_ops, 'IX')
    dd_ops = OR_iIXY_d(dd_ops, 'IX')
    dd_ops = XOR_iIXY_d(dd_ops, 'IX')
    dd_ops = INC_iIXY_d(dd_ops, 'IX')
    dd_ops = DEC_iIXY_d(dd_ops, 'IX')
    dd_ops = INC_IXY(dd_ops, 'IX')
    dd_ops = DEC_IXY(dd_ops, 'IX')
    dd_ops = JP_iIXY(dd_ops, 'IX')
    dd_ops = ADD_IXY_pp(dd_ops, 'IX')
    dd_ops = DD_FD_CB(dd_ops, 0xDD)
    # undocumented instructions
    dd_ops = uALU_IXYhl(dd_ops, 'IX')
    dd_ops = uINC_IXYhl(dd_ops, 'IX')
    dd_ops = uDEC_IXYhl(dd_ops, 'IX')
    dd_ops = uLD_r_IXYhl(dd_ops, 'IX')
    dd_ops = uLD_IXYhl_n(dd_ops, 'IX')
    return dd_ops

#-------------------------------------------------------------------------------
def gen_fd_opcodes() :
    '''
    Generate the FD-lead-byte opcode table.
    '''
    fd_ops = {}
    fd_ops = LD_r_iIXY_d(fd_ops, 'IY')
    fd_ops = LD_iIXY_d_r(fd_ops, 'IY')
    fd_ops = LD_iIXY_d_n(fd_ops, 'IY')
    fd_ops = LD_IXY_nn(fd_ops, 'IY')
    fd_ops = LD_IXY_inn(fd_ops, 'IY')
    fd_ops = LD_inn_IXY(fd_ops, 'IY')
    fd_ops = LD_SP_IXY(fd_ops, 'IY')
    fd_ops = PUSH_IXY(fd_ops, 'IY')
    fd_ops = POP_IXY(fd_ops, 'IY')
    fd_ops = EX_iSP_HLIXY(fd_ops, 'IY')
    fd_ops = ADD_iIXY_d(fd_ops, 'IY')
    fd_ops = ADC_iIXY_d(fd_ops, 'IY')
    fd_ops = SUB_iIXY_d(fd_ops, 'IY')
    fd_ops = CP_iIXY_d(fd_ops, 'IY')
    fd_ops = SBC_A_iIXY_d(fd_ops, 'IY')
    fd_ops = AND_iIXY_d(fd_ops, 'IY')
    fd_ops = OR_iIXY_d(fd_ops, 'IY')
    fd_ops = XOR_iIXY_d(fd_ops, 'IY')
    fd_ops = INC_iIXY_d(fd_ops, 'IY')
    fd_ops = DEC_iIXY_d(fd_ops, 'IY')
    fd_ops = INC_IXY(fd_ops, 'IY')
    fd_ops = DEC_IXY(fd_ops, 'IY')
    fd_ops = JP_iIXY(fd_ops, 'IY')
    fd_ops = ADD_IXY_pp(fd_ops, 'IY')
    fd_ops = DD_FD_CB(fd_ops, 0xFD)
    # undocumented instructions
    fd_ops = uALU_IXYhl(fd_ops, 'IY')
    fd_ops = uINC_IXYhl(fd_ops, 'IY')
    fd_ops = uDEC_IXYhl(fd_ops, 'IY')
    fd_ops = uLD_r_IXYhl(fd_ops, 'IY')
    fd_ops = uLD_IXYhl_n(fd_ops, 'IY')
    return fd_ops

#-------------------------------------------------------------------------------
def gen_ed_opcodes() :
    '''
    Generate the ED-lead-byte opcode table.
    '''
    ed_ops = {}
    ed_ops = LD_A_I(ed_ops)
    ed_ops = LD_I_A(ed_ops)
    ed_ops = LD_A_R(ed_ops)
    ed_ops = LD_R_A(ed_ops)
    ed_ops = LD_dd_inn(ed_ops)
    ed_ops = LD_inn_dd(ed_ops)
    ed_ops = RLD(ed_ops)
    ed_ops = RRD(ed_ops)
    ed_ops = LDI(ed_ops)
    ed_ops = LDIR(ed_ops)
    ed_ops = LDD(ed_ops)
    ed_ops = LDDR(ed_ops)
    ed_ops = CPI(ed_ops)
    ed_ops = CPIR(ed_ops)
    ed_ops = CPD(ed_ops)
    ed_ops = CPDR(ed_ops)
    ed_ops = NEG(ed_ops)
    ed_ops = IM(ed_ops)
    ed_ops = IN_r_iC(ed_ops)
    ed_ops = INI(ed_ops)
    ed_ops = INIR(ed_ops)
    ed_ops = IND(ed_ops)
    ed_ops = INDR(ed_ops)
    ed_ops = OUT_iC_r(ed_ops)
    ed_ops = OUTI(ed_ops)
    ed_ops = OTIR(ed_ops)
    ed_ops = OUTD(ed_ops)
    ed_ops = OTDR(ed_ops)
    ed_ops = ADC_HL_ss(ed_ops)
    ed_ops = SBC_HL_ss(ed_ops)
    ed_ops = RETI(ed_ops)
    return ed_ops

#-------------------------------------------------------------------------------
def gen_source(f, ops, ext_ops) :
    '''
    Generate the source code from opcode table.
    '''
    
    # write header
    f.write('#pragma once\n')
    f.write('// #version:{}#\n'.format(Version))
    f.write('// machine generated, do not edit!\n')
    f.write('#include "core/common.h"\n')
    f.write('namespace yakc {\n')
    f.write('inline unsigned int z80::step() {\n')
    f.write('    int d;\n')
    f.write('    uword u16tmp;\n')
    f.write('    state.INV = false;\n')
    f.write('    if (enable_interrupt) {\n')
    f.write('        state.IFF1 = state.IFF2 = true;\n')
    f.write('        enable_interrupt = false;\n')
    f.write('    }\n')
    f.write('    switch (fetch_op()) {\n')
    
    # generate the switch case in sorted order
    for op in range(0,256) :
        if op in ops :
            f.write('    case {}:\n'.format(hex(op)))
            if op in [ 0xCB, 0xDD, 0xFD, 0xED ] :
                f.write('        switch (fetch_op()) {\n')
                for ext_op in range(0,256) :
                    if ext_op in ext_ops[op] :
                        f.write('        case {}:\n'.format(hex(ext_op)))
                        for line in ext_ops[op][ext_op] :
                            f.write('            {}\n'.format(line))
                        f.write('            break;\n')
                f.write('        default:\n')
                f.write('            return invalid_opcode(2);\n')
                f.write('            break;\n')
                f.write('        }\n')
            else :
                for line in ops[op] :
                    f.write('        {}\n'.format(line))
            f.write('        break;\n')
    f.write('    default:\n')
    f.write('       return invalid_opcode(1);\n')
    f.write('       break;\n')
    f.write('    }\n')
    f.write('#ifdef _DEBUG\n')
    f.write('    YAKC_ASSERT(false);\n')
    f.write('#endif\n')
    f.write('    return 0;\n')
    f.write('}\n')
    f.write('} // namespace yakc\n')

#-------------------------------------------------------------------------------
def generate(input, out_src, out_hdr) :
    if genutil.isDirty(Version, [input], [out_hdr]) :
        ops = gen_opcodes()
        ext_ops = {}
        ext_ops[0xCB] = gen_cb_opcodes()
        ext_ops[0xDD] = gen_dd_opcodes()
        ext_ops[0xFD] = gen_fd_opcodes()
        ext_ops[0xED] = gen_ed_opcodes()
        with open(out_hdr, 'w') as f:
            gen_source(f, ops, ext_ops)


