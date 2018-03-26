## Wolfgang Lorenz Test Suite

### Howto

- the test binaries are under **yakc/files/wlorenz/**
- load them via the 'Load File' window
- the test entry address is $801 (2049)
- start the currently loaded test with **SYS 2049**

### Test Status

- adca: OK
- adcax: OK
- adcay: OK
- adcb OK
- adcix: OK
- adciy: OK
- adcz: OK
- alrb: OK
- ancb: OK
- anda: OK
- andax: OK
- anday: OK
- andb: OK
- andix: OK
- andiy: OK
- andz: OK
- andzx: OK
- **aneb FAIL**
- arrb: OK
- asla: OK
- aslax: OK
- asln: OK
- aslz: OK
- aslzx: OK
- asoa: OK
- asoax: OK
- asoay: OK
- asoix: OK
- asoiy: OK
- asoz: OK
- asozx: OK
- axsa: OK
- axsix: OK
- axsz: OK
- axszy: OK
- bccr: OK
- bcsr: OK
- beqr: OK
- bita: OK
- bitz: OK
- bmir: OK
- bner: OK
- bplr: OK
- branchwrap: OK
- brkn: OK, but doesn't finish?
- bvcr: OK
- bvsr: OK
- cia1pb6: FAIL
    09d0: sta $dc01 (A = BF)
          cmp $dc01
          -> check if value written to port is same as read


- cia1pb7: FAIL
- cia1ta: FAIL
    INIT:   I4 B4 IE BE
            1E 00 10 11 -> FORCE-LOAD + START
    AFTER:  A4 AD AE
            FF 00 01
    RIGHT:  00 81 01
                                AD: INT-CONTROL AFTER
                                AE: CRA AFTER
- cia1tab: FAIL
- cia1tb: FAIL
- cia...
- clcn OK
- cldn OK
- clin OK
- clvn OK
- cmpa OK
- cmpax OK
- cmpay OK
- cmpb OK
- cmpix OK
- cmpiy OK
- cmpz OK
- cmpzx OK
- cntdef FAIL   (requires cascaded CIA timers)
- cnto2 FAIL    (also CIA counter-related)
- cpuport OK
- cputiming FAIL (undocumented opcodes 5C and 7C are 1 off)
...
- flipos FAIL (CIA timer related)
- icr01 FAIL (CIA ICR related)





OK:
cpuport

FAILS:

cia1pb6
cia1pb7
cia1ta
cntdef
cnto2

cputiming:
    5C clock 8 right 9
    7C clock 8 right 9

icr01
    CIA1 ICR is not $01
    READING ICR=01 DID NOT CLEAR INT
    CIA2 ICR is not $02
    READING ICR=01 DID NOT CLEAR ICR
    READING ICR=01 DID NOT PREVENT NMI
    READING ICR=00 MAY NOT CLEAR ICR
    READING ICR=00 MAY NOT PREVENT NMI

imr
    IMR=$81 NO IRQ IN CLOCK 3
    IMR=$7F MAY NOT CLEAR INT

irq
    WRONG DC0D
    01/5
    STACK 1084 00 14
    RIGHT 1085 00 14
    03/7
    STACK 10C4 00 14
    RIGHT 10C5 00 14
    ...

mmu
mmufetch ????
nmi
    WRONG $DD0D
    01/5
    STACK 105B 00 14
    RIGHT 105C 00 14
oneshot
    CRA IS NOT $08 AT ICR=$01
rtin ???
trap1
    SHA 93
        AFTER 1B C6 B1 6C 30
        RIGHT 00 C6 B1 6C 30
    ...
