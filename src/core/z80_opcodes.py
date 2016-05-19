#-------------------------------------------------------------------------------
#   z80_opcodes.py
#   Generate brute-force Z80 instruction decoder.
#   See: http://www.z80.info/decoding.htm
#-------------------------------------------------------------------------------

# 8-bit register table, the 'HL' entry means '(HL)', and may be patched
# to IX or IY for DD/FD prefix instructions
r = [
    'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A'
]

# the same, but the '(HL)' item is never patched to IX/IY
r2 = [
    'B', 'C', 'D', 'E', 'H', 'L', 'HL', 'A'
]

# 16-bit register table, with SP
rp = [
    'BC', 'DE', 'HL', 'SP'
]

# 16-bit register table, with AF
rp2 = [
    'BC', 'DE', 'HL', 'AF'
]

# the condition-code table (for conditional jumps etc)
cc = [
    '!(F & ZF)',        # NZ
    '(F & ZF)',         # Z
    '!(F & CF)',        # NC
    '(F & CF)',         # C
    '!(F & PF)',        # PO
    '(F & PF)',         # PE
    '!(F & SF)',        # P
    '(F & SF)'          # M
]

# an 'op' consists of the opcode-byte, the C++ code, and the number of cycles,
# ops are collected in several lookup tables, invalid ops have no code and
# 0 cycles
class item :
    def __init__(self, op) :
        self.op  = op
        self.cmt = None
        self.src = None
        self.cyc = 0

# op lookup tables
items = []        # main instructions
cb_items = []     # CB prefix instructions
dd_items = []     # DD prefix instructions
ed_items = []     # ED prefix instructions
fd_items = []     # FD prefix instructions 
dd_cb_items = []  # DD CB prefix instructions
fd_cb_items = []  # FD CB prefix instructions

# helper function, return comment for (HL), (IX+d), (IY+d)
def iHLcmt(ext) :
    if (ext) :
        return '({}+d)'.format(r[6])
    else :
        return '({})'.format(r[6])

# helper function, return code for address of (HL), (IX+d), (IY+d)
def iHLsrc(ext) :
    if (ext) :
        # IX+d or IY+d
        return '{}+mem.rs8(PC++)'.format(r[6])
    else :
        # HL
        return '{}'.format(r[6])

# encode a main instruction, this may also be an DD or FD prefix instruction
# takes an opcode byte and returns an opcode object with code and 
# number of cycles
def enc_op(op, ext) :

    o = item(op)

    x = op>>6
    y = (op>>3)&7
    z = op&7
    ext_cycles = 8 if ext else 0

    # 8-bit loads, and special case HALT
    if x == 1 :
        if y == 6 :
            if z == 6 :
                # special case LD (HL),(HL) is HALT
                o.cmt = 'HALT'
                o.src = 'halt();'
                o.cyc = 4
            else :
                # LD (HL),r; LD (IX+d),r; LD (IX+d),r
                o.cmt = 'LD {},{}'.format(iHLcmt(ext), r2[z])
                o.src = 'mem.w8({}, {});'.format(iHLsrc(ext), r2[z])
                o.cyc = 7 + ext_cycles
        elif z == 6 :
            # LD r,(HL); LD r,(IX+d); LD r,(IY+d)
            o.cmt = 'LD {},{}'.format(r2[y], iHLcmt(ext))
            o.src = '{} = mem.r8({});'.format(r2[y], iHLsrc(ext))
            o.cyc = 7 + ext_cycles
        else :
            # LD r,s
            o.cmt = 'LD {},{}'.format(r2[y], r2[z])
            o.src = '{} = {};'.format(r2[y], r2[z])
            o.cyc = 4

    return o

# main encoder function, this populates all the opcode tables
def encode_all() :

    # main instructions
    for i in range(0, 256) :
        o = enc_op(i, False)
        if o.src :
            print('{}: {} = {}:\n  {}\n'.format(hex(o.op), o.cmt, o.cyc, o.src))

    # DD instructions
    r[6] = 'IX'
    for i in range(0, 256) :
        o = enc_op(i, True)
        o.cyc += 4
        if o.src :
            print('0xDD {}: {} = {}:\n  {}\n'.format(hex(o.op), o.cmt, o.cyc, o.src))

    # FD instructions
    r[6] = 'IY'
    for i in range(0, 256) :
        o = enc_op(i, True)
        o.cyc += 4
        if o.src :
            print('0xFD {}: {} = {}:\n  {}\n'.format(hex(o.op), o.cmt, o.cyc, o.src))

# __main__
if __name__ == "__main__" :
    encode_all()

