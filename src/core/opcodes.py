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
    CC(0b000, 'NZ', '(!(F & ZF))'),
    CC(0b001, 'Z',  '(F & ZF)'),
    CC(0b010, 'NC', '(!(F & CF))'),
    CC(0b011, 'C',  '(F & CF)'),
    CC(0b100, 'PO', '(!(F & PF))'),
    CC(0b101, 'PE', '(F & PF)'),
    CC(0b110, 'P',  '(!(F & SF))'),
    CC(0b111, 'M',  '(F & SF)')
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
    for im in {(0,0x46),(1,0x56),(2,0x5E)} :
        src = [
            '// IM {}'.format(im[0]),
            'IM = {};'.format(im[0]),
            t(8)]
        ops = add_op(ops, im[1], src)
    return ops

#-------------------------------------------------------------------------------
def RST(ops) :
    for p in range(0,8) :
        src = [
            '// RST {}'.format(p),
            'rst({});'.format(p<<3),
            t(11)]
        ops = add_op(ops, 0b11000111|p<<3, src)
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
                lhs = '{}'.format(r.name)
            if s.mem :
                rhs = 'mem.r8(HL)'
                cycles = 7
            else :
                rhs = '{}'.format(s.name)
            src = [ '// LD {},{}'.format(r.name, s.name) ]
            if r.mem :
                src.append('mem.w8(HL, {});'.format(rhs))
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
            lhs = '{}'.format(r.name)
            cycles = 7
        rhs = 'mem.r8(PC++)'
        src = [ '// LD {},n'.format(r.name) ]
        if r.mem :
            src.append('mem.w8(HL, {});'.format(rhs))
        else :
            src.append('{} = mem.r8(PC++);'.format(lhs))
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
                '{} = {};'.format(rname, sname),
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
            '{}{} = mem.r8(PC++);'.format(xname, r.name),
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
        'A = mem.r8(BC);',
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
        'mem.w8(BC, A);',
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
        'A = mem.r8(DE);',
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
        'mem.w8(DE, A);',
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
        'A = mem.r8(mem.r16(PC));',
        'PC += 2;',
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
        'mem.w8(mem.r16(PC), A);',
        'PC += 2;',
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
        'A = I;',
        'F = sziff2(I,IFF2)|(F&CF);',
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
        'I = A;',
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
        'A = R;',
        'F = sziff2(R,IFF2)|(F&CF);',
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
        'R = A;',
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
            '{} = mem.r16(PC);'.format(r.name),
            'PC += 2;',
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
        'HL = mem.r16(mem.r16(PC));',
        'PC += 2;',
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
        'mem.w16(mem.r16(PC), HL);',
        'PC += 2;',
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
            '{} = mem.r16(mem.r16(PC));'.format(r.name),
            'PC += 2;',
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
            'mem.w16(mem.r16(PC), {});'.format(r.name),
            'PC += 2;',
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
        'SP = HL;',
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
            'SP -= 2;',
            'mem.w16(SP, {});'.format(r.name),
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
            '{} = mem.r16(SP);'.format(r.name),
            'SP += 2;',
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
        'SP -= 2;',
        'mem.w16(SP, {});'.format(xname),
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
        '{} = mem.r16(SP);'.format(xname),
        'SP += 2;',
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
            'd = mem.rs8(PC++);',
            '{} = mem.r8({} + d);'.format(r.name, xname),
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
        'd = mem.rs8(PC++);',
        'mem.w8({} + d, mem.r8(PC++));'.format(xname),
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
            'd = mem.rs8(PC++);',
            'mem.w8({} + d, {});'.format(xname, r.name),
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
        '{} = mem.r16(PC);'.format(xname),
        'PC += 2;',
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
        '{} = mem.r16(mem.r16(PC));'.format(xname),
        'PC += 2;',
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
        'mem.w16(mem.r16(PC), {});'.format(xname),
        'PC += 2;',
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
        'SP = {};'.format(xname),
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
        'swap16(DE, HL);',
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
        'swap16(AF, AF_);',
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
        'swap16(BC, BC_);',
        'swap16(DE, DE_);',
        'swap16(HL, HL_);',
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
        'u16tmp = mem.r16(SP);',
        'mem.w16(SP, {});'.format(xname),
        '{} = u16tmp;'.format(xname),
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
            if r.mem :
                rhs = 'mem.r8(HL)'
                cycles = 7
            else :
                rhs = '{}'.format(r.name)
                cycles = 4
            src = [
                '// {} {}'.format(alu[0], r.name),
                '{}({});'.format(alu[1], rhs),
                t(cycles)]
            ops = add_op(ops, alu[2]|r.bits, src)
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
            '{}(mem.r8(PC++));'.format(alu[1]),
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
            src = [
                '// {} {}{}'.format(alu[0], xname, r.name),
                '{}({}{});'.format(alu[1], xname, r.name),
                t(8)]
            ops = add_op(ops, alu[2]|r.bits, src)
    return ops

#-------------------------------------------------------------------------------
def ALU_iIXY_d(ops, xname) :
    alu_ops = [
        ['ADD', 'add8', 0x86],
        ['ADC', 'adc8', 0x8E],
        ['SUB', 'sub8', 0x96],
        ['SBC', 'sbc8', 0x9E],
        ['AND', 'and8', 0xA6],
        ['XOR', 'xor8', 0xAE],
        ['OR',  'or8',  0xB6],
        ['CP',  'cp8',  0xBE]
    ]
    for alu in alu_ops :
        src = [
            '// {}({}+d)'.format(alu[0], xname),
            'd = mem.rs8(PC++);',
            '{}(mem.r8({} + d));'.format(alu[1], xname),
            t(19)]
        ops = add_op(ops, alu[2], src)
    return ops

#-------------------------------------------------------------------------------
def INC_DEC_r(ops) :
    inc_dec_ops = [
        ['INC', 'inc8', 0b00000100],
        ['DEC', 'dec8', 0b00000101]
    ]
    for id_op in inc_dec_ops :
        for r in r8ihl :
            src = ['// {} {}'.format(id_op[0], r.name)]
            if r.mem :
                src.append('mem.w8(HL, {}(mem.r8(HL)));'.format(id_op[1]))
                src.append(t(11))
            else :
                cycles = 4
                src.append('{} = {}({});'.format(r.name, id_op[1], r.name))
                src.append(t(4))
            ops = add_op(ops, id_op[2]|r.bits<<3, src)
    return ops

#-------------------------------------------------------------------------------
def uINC_DEC_IXYhl(ops, xname) :
    '''
    undocumented!
    '''
    inc_dec_ops = [
        ['INC', 'inc8', 0b00000100],
        ['DEC', 'dec8', 0b00000101]
    ]
    for id_op in inc_dec_ops :
        for r in r8ixy:
            src = [
                '// {} {}{}'.format(id_op[0], xname, r.name),
                '{}{} = {}({}{});'.format(
                    xname, r.name, id_op[1], xname, r.name),
                t(8)]
            ops = add_op(ops, id_op[2]|r.bits<<3, src)
    return ops

#-------------------------------------------------------------------------------
def INC_DEC_iIXY_d(ops, xname) :
    inc_dec_ops = [
        ['INC', 'inc8', 0x34],
        ['DEC', 'dec8', 0x35]
    ]
    for id_op in inc_dec_ops :
        src = [
            '// {} ({}+d)'.format(id_op[0], xname),
            'd = mem.rs8(PC++);',
            'mem.w8({}+d, {}(mem.r8({}+d)));'.format(
                xname, id_op[1], xname),
            t(23)]
        ops = add_op(ops, id_op[2], src)
    return ops

#-------------------------------------------------------------------------------
def INC_DEC_ss(ops) :
    inc_dec_ops = [
        ['INC', '++', 0b00000011],
        ['DEC', '--', 0b00001011]
    ]
    for id_op in inc_dec_ops :
        for r in r16sp :
            src = [
                '// {} {}'.format(id_op[0], r.name),
                '{}{};'.format(r.name, id_op[1]),
                t(6)]
            ops = add_op(ops, id_op[2]|r.bits<<4, src)
    return ops

#-------------------------------------------------------------------------------
def INC_DEC_IXY(ops, xname) :
    inc_dec_ops = [
        ['INC', '++', 0x23],
        ['DEC', '--', 0x2B]
    ]
    for id_op in inc_dec_ops :
        src = [
            '// {} {}'.format(id_op[0], xname),
            '{}{};'.format(xname, id_op[1]),
            t(10)]
        ops = add_op(ops, id_op[2], src)
    return ops

#-------------------------------------------------------------------------------
def RLCA_RRCA_RLA_RRA(ops) :
    r_ops = [
        ['RLCA', 'rlc8', 0x07],
        ['RRCA', 'rrc8', 0x0F],
        ['RLA',  'rl8',  0x17],
        ['RRA',  'rr8',  0x1F]
    ]
    for r_op in r_ops :
        src = [
            '// {}'.format(r_op[0]),
            'A = {}(A, false);'.format(r_op[1]),
            t(4)]
        ops = add_op(ops, r_op[2], src)
    return ops

#-------------------------------------------------------------------------------
def ROT_r(ops) :
    rs_ops = [
        ['RLC', 'rlc8', 0b00000000],
        ['RRC', 'rrc8', 0b00001000],
        ['RL',  'rl8',  0b00010000],
        ['RR',  'rr8',  0b00011000]
    ]
    for rs_op in rs_ops :
        for r in r8ihl :
            src = ['// {} {}'.format(rs_op[0], r.name)]
            if r.mem :
                src.append('mem.w8(HL,{}(mem.r8(HL), true));'.format(
                    rs_op[1]))
                cycles = 15
            else :
                src.append('{} = {}({}, true);'.format(
                    r.name, rs_op[1], r.name))
                cycles = 8
            src.append(t(cycles))
            ops = add_op(ops, rs_op[2]|r.bits, src)
    return ops

#-------------------------------------------------------------------------------
def SHFT_r(ops) :
    shft_ops = [
        ['SLA', 'sla8', 0b00100000],
        ['SLL', 'sll8', 0b00110000],
        ['SRA', 'sra8', 0b00101000],
        ['SRL', 'srl8', 0b00111000]
    ]
    for sh_op in shft_ops :
        for r in r8ihl :
            src = ['// {} {}'.format(sh_op[0], r.name)]
            if r.mem :
                src.append('mem.w8(HL,{}(mem.r8(HL)));'.format(
                    sh_op[1]))
                cycles = 15
            else :
                src.append('{} = {}({});'.format(
                    r.name, sh_op[1], r.name))
                cycles = 8
            src.append(t(cycles))
            ops = add_op(ops, sh_op[2]|r.bits, src)
    return ops

#-------------------------------------------------------------------------------
def RLD_RRD(ops) :
    ops = add_op(ops, 0x6F, ['// RLD', 'rld();', t(18)])
    ops = add_op(ops, 0x67, ['// RRD', 'rrd();', t(18)])
    return ops

#-------------------------------------------------------------------------------
def BIT_b_r(ops) :
    for b in range(0,8) :
        for r in r8ihl :
            src = ['// BIT {},{}'.format(b, r.name)]
            if r.mem :
                src.append('bit(mem.r8(HL),(1<<{}));'.format(b))
                src.append(t(12))
            else :
                src.append('bit({},(1<<{}));'.format(r.name, b))
                src.append(t(8))
            ops = add_op(ops, 0b01000000|b<<3|r.bits, src)
    return ops

#-------------------------------------------------------------------------------
def SET_RES_b_r(ops) :
    sr_ops = [
        ['SET', '|',  0b11000000],
        ['RES', '&~', 0b10000000]
    ]
    for sr_op in sr_ops :
        for b in range(0,8) :
            for r in r8ihl :
                src = ['// {} {},{}'.format(sr_op[0], b, r.name)]
                if r.mem :
                    src.append('mem.w8(HL,mem.r8(HL){}(1<<{}));'.format(
                        sr_op[1], b))
                    src.append(t(15))
                else :
                    src.append('{} = {}{}(1<<{});'.format(
                        r.name, r.name, sr_op[1], b))
                    src.append(t(8))
                ops = add_op(ops, sr_op[2]|b<<3|r.bits, src)
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
def LD_CP_I_D_R(ops) :
    src = add_op(ops, 0xA0, ['// LDI', 'ldi();', t(16)])
    src = add_op(ops, 0xA1, ['// CPI', 'cpi();', t(16)])
    src = add_op(ops, 0xA8, ['// LDD', 'ldd();', t(16)])
    src = add_op(ops, 0xA9, ['// CPD', 'cpd();', t(16)])
    src = add_op(ops, 0xB0, ['// LDIR', 'return ldir();'])
    src = add_op(ops, 0xB1, ['// CPIR', 'return cpir();'])
    src = add_op(ops, 0xB8, ['// LDDR', 'return lddr();'])
    src = add_op(ops, 0xB9, ['// CPDR', 'return cpdr();'])
    return src

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
        'A ^= 0xFF;',
        'F = (F&(SF|ZF|PF|CF)) | HF | NF | (A&(YF|XF));',
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
        'F = ((F&(SF|ZF|YF|XF|PF|CF))|((F&CF)<<4)|(A&(YF|XF)))^CF;',
        t(4)]
    return add_op(ops, 0x3F, src)

#-------------------------------------------------------------------------------
def SCF(ops) :
    '''
    SCF
    T-states: 4
    '''
    src = ['// SCF',
        'F = (F&(SF|ZF|YF|XF|PF))|CF|(A&(YF|XF));',
        t(4)]
    return add_op(ops, 0x37, src)

#-------------------------------------------------------------------------------
def JP_nn(ops) :
    '''
    JP nn
    T-states: 10
    '''
    src = ['// JP nn',
        'PC = mem.r16(PC);',
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
            'PC = {} ? mem.r16(PC) : PC+2;'.format(c.test),
            t(10)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def JR_e(ops) :
    '''
    JR e
    T-states: 12
    '''
    src = ['// JR e', 'PC += mem.rs8(PC) + 1;', t(12)]
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
        CC(0x38, 'C',  'F & CF'),
        CC(0x30, 'NC', '!(F & CF)'),
        CC(0x28, 'Z',  'F & ZF'),
        CC(0x20, 'NZ', '!(F & ZF)')
    ]
    for c in cc :
        src = ['// JR {},e'.format(c.name),
            'if ({}) {{'.format(c.test),
            '    PC += mem.rs8(PC) + 1;',
            '    return 12;',
            '}',
            'else {',
            '    PC++;',
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
    src = ['// JP (HL)', 'PC = HL;', t(4)]
    return add_op(ops, 0xE9, src)

#-------------------------------------------------------------------------------
def JP_iIXY(ops, xname) :
    '''
    JP ([IX|IY])
    T-states: 8
    '''
    src = ['// JP ({})'.format(xname), 'PC = {};'.format(xname), t(8)]
    return add_op(ops, 0xE9, src)

#-------------------------------------------------------------------------------
def DJNZ_e(ops) :
    '''
    DJNZ e
    T-states: 13/8
    '''
    src = [
        '// DJNZ e',
        'if (--B > 0) {',
        '    PC += mem.rs8(PC) + 1;',
        '    return 13;',
        '}',
        'else {',
        '    PC++;',
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
        'SP -= 2;',
        'mem.w16(SP, PC+2);',
        'PC = mem.r16(PC);',
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
            '    SP -= 2;',
            '    mem.w16(SP, PC+2);',
            '    PC = mem.r16(PC);',
            '    return 17;',
            '}',
            'else {',
            '    PC += 2;',
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
        'PC = mem.r16(SP);',
        'SP += 2;',
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
            '    PC = mem.r16(SP);',
            '    SP += 2;',
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
    src = ['// IN A,(n)', 'A = in((A<<8)|(mem.r8(PC++)));', t(11)]
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
            '{} = in(BC);'.format(r.name),
            'F = szp[{}]|(F&CF);'.format(r.name),
            t(12)]
        add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def IN_OUT_I_D_R(ops) :
    src = add_op(ops, 0xA2, ['// INI', 'ini();', t(16)])
    src = add_op(ops, 0xA3, ['// OUTI', 'outi();', t(16)])
    src = add_op(ops, 0xAA, ['// IND', 'ind();', t(16)])
    src = add_op(ops, 0xAB, ['// OUTD', 'outd();', t(16)])
    src = add_op(ops, 0xB2, ['// INIR', 'return inir();'])
    src = add_op(ops, 0xB3, ['// OTIR', 'return otir();'])
    src = add_op(ops, 0xBA, ['// INDR', 'return indr();'])
    src = add_op(ops, 0xBB, ['// OTDR', 'return otdr();'])
    return src

#-------------------------------------------------------------------------------
def OUT_in_A(ops) :
    '''
    OUT (n),A
    T-states: 11
    '''
    src = ['// OUT (n),A', 'out((A<<8)|mem.r8(PC++),A);', t(11)]
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
            'out(BC, {});'.format(r.name),
            t(12)]
        ops = add_op(ops, op, src)
    return ops

#-------------------------------------------------------------------------------
def ADD_HL_ss(ops) :
    '''
    ADD HL,ss
    T-states: 11
    '''
    for r in r16sp :
        op = 0b00001001 | r.bits<<4
        src = ['// ADD HL,{}'.format(r.name),
            'HL = add16(HL, {});'.format(r.name),
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
            'HL = adc16(HL, {});'.format(r.name),
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
            'HL = sbc16(HL, {});'.format(r.name),
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
            '{} = add16({}, {});'.format(xname, xname, r.name),
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
    ops = RST(ops)
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
    ops = INC_DEC_r(ops)
    ops = INC_DEC_ss(ops)
    ops = RLCA_RRCA_RLA_RRA(ops)
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
    cb_ops = ROT_r(cb_ops)
    cb_ops = SHFT_r(cb_ops)
    cb_ops = BIT_b_r(cb_ops)
    cb_ops = SET_RES_b_r(cb_ops)
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
    dd_ops = ALU_iIXY_d(dd_ops, 'IX')
    dd_ops = INC_DEC_iIXY_d(dd_ops, 'IX')
    dd_ops = INC_DEC_IXY(dd_ops, 'IX')
    dd_ops = JP_iIXY(dd_ops, 'IX')
    dd_ops = ADD_IXY_pp(dd_ops, 'IX')
    dd_ops = DD_FD_CB(dd_ops, 0xDD)
    # undocumented instructions
    dd_ops = uALU_IXYhl(dd_ops, 'IX')
    dd_ops = uINC_DEC_IXYhl(dd_ops, 'IX')
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
    fd_ops = ALU_iIXY_d(fd_ops, 'IY')
    fd_ops = INC_DEC_iIXY_d(fd_ops, 'IY')
    fd_ops = INC_DEC_IXY(fd_ops, 'IY')
    fd_ops = JP_iIXY(fd_ops, 'IY')
    fd_ops = ADD_IXY_pp(fd_ops, 'IY')
    fd_ops = DD_FD_CB(fd_ops, 0xFD)
    # undocumented instructions
    fd_ops = uALU_IXYhl(fd_ops, 'IY')
    fd_ops = uINC_DEC_IXYhl(fd_ops, 'IY')
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
    ed_ops = RLD_RRD(ed_ops)
    ed_ops = LD_CP_I_D_R(ed_ops)
    ed_ops = NEG(ed_ops)
    ed_ops = IM(ed_ops)
    ed_ops = IN_r_iC(ed_ops)
    ed_ops = IN_OUT_I_D_R(ed_ops)
    ed_ops = OUT_iC_r(ed_ops)
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
    f.write('    INV = false;\n')
    f.write('    if (enable_interrupt) {\n')
    f.write('        IFF1 = IFF2 = true;\n')
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


