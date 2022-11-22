# Arm & RISC-V

## Reference

* [Arm32](https://developer.arm.com/documentation/ddi0597/latest)
* [Arm64](https://developer.arm.com/documentation/ddi0596/2021-09)
* [RISC-V](https://msyksphinz-self.github.io/riscv-isadoc/html/index.html)

## Contents

### Microcomputer

Nothing

### LED flashing

Nothing

### Register set

* Arm32
  * (R0 ~ R15) + APSR
  * R13 = SP
  * R14 = LR
  * R15 = PC
* Arm64
  * (X0 ~ X30) + SP + ZR + PC + PSTATE
  * Xn ... 64bit
  * Wn ... 32bit
* Arm32(in Arm64)
  * (R0 ~ R14) + PC + APSR
* RISC-V
  * (X0 ~ X31) + PC + (m/s/u)status
  * X0 = ZR
  * X1 = RA
  * X2 = SP
  * X3 = GP
  * X4 = TP

### Instuctions

* MOVE
  * Arm32: MOV, MOV+ORR, MOVW+MOVT / (Address) LDR, ADR
  * Arm64: MOV, MOVK+ lsl / (Address) LDR, ADR, ADRP
  * RISC-V: OR, ADD, LUI+ADD, LI / (Address) LA, AUIPC+ADDI, AUIPC+LW
* LOAD/STR
  * Arm32: LDRB, LDRSB, LDRH, LDRSH, LDR / STRB, STRH, STR
  * Arm64:
    * LDRB, LDRH, LDR, LDRSB, LDRSH, LDRSW / STRB, STRH, STR
    * For AArch32
      * LDURB, LDURH, LDUR, LDURSW / STURB, STURH, STUR
      * ...
  * RISC-V: 
    * LB, LH, LW, LBU, LHU / SB, SH, SW
* LOOP and Conditions
  * Arm32:
    * Flags: Z[ero], N[egative], C[arry], V[...overflow]
    * Conditions: EQ, NE, CS/HS, CC/LO, MI, PL, VS, VC, HI, LS, GE, LT, GT, LE, (AL)
    * CMP, TST, ADDS, SUBS
    * ADD, SUB
  * Arm64:
    * ~~CMP~~, TST, ADDS, SUBS, CBZ, CBNZ, TBZ, TBNZ
  * RISC-V: 
    * Flags: None
    * BEQ, BNE, BLT, BGE, BLTU, BGEU

