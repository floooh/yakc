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
- brkn: OK
- bvcr: OK
- bvsr: OK
- cia1pb6: FAIL
    WRITING TA HIGH MAY NOT SET P6B HIGH
    PB6 TOGGLE TIMED OUT
- cia1pb7: OK
- cia1ta: FAIL
    1 or 2 cycles off
- cia1tab: OK
- cia1tb: FAIL
    1 or 2 cycles off
- cia1tb13: FAIL
    ERROR 00 10 CYCLE 2
    ERROR 00 10 CYCLE 3
    ...
- cia2pb6: FAIL
    WRITING TA HIGH MAY NOT SET P6B HIGH
- cia2pb7: OK
- cia2ta: FAIL
    1 or 2 cycles off
- cia2tb: FAIL
- cia2tb123: FAIL
    ERROR 00 10 CYCLE 2
    ERROR 00 10 CYCLE 3
    ...

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
- cntdef OK
- cnto2 OK
- cpuport OK
- cputiming FAIL (undocumented opcodes 5C and 7C are 1 off)
...
- flipos OK
- icr01 FAIL (WORKED BEFORE!)
    READING ICR=81 MUST PASS NMI
- imr OK
...
- mmu FAIL
- mmufetch ???
- nmi FAIL
...
- oneshot OK
