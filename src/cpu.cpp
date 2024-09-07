#include "cpu.h"

/**
 * Construct a new CPU.
 * @param bus The bus to attach to.
 */
Cpu::Cpu(Bus *bus) {
    this->bus = bus;
}

/**
 * Reset the CPU.
 */
void Cpu::reset(uint32_t program_counter, uint32_t dtb_base) {
    memset(x, 0, sizeof(x));
    memset(csr, 0, sizeof(csr));

    x[11] = dtb_base;
    csr[MISA] = 0x40401101;
    csr[MVENDORID] = 0x12345678;
    timer_h = 0;
    timer_l = 0;
    timer_trigger_h = 0;
    timer_trigger_l = 0;
    op_mode = OPMODE_MACHINE;
    wfi_bit = false;
    pc = program_counter;
}

/**
 * Trigger a reset.
 */
void Cpu::triggerReset() {
    reset_triggered = true;
}

/**
 * Execute a number of instructions.
 * @param num_instructions The number of instructions to execute.
 * @param elapsed_micros The number of microseconds elapsed.
 * @return 1 if return reason is reset, 0 otherwise.
 */
bool Cpu::execute(uint32_t num_instructions, uint32_t elapsed_micros) {
    reset_triggered = false;
    timer_h += (timer_l + elapsed_micros) < timer_l;
    timer_l += elapsed_micros;

    // Fire Timer interrupt if timer is enabled and has triggered
    if ((timer_trigger_h || timer_trigger_l) && (timer_h >= timer_trigger_h) && (timer_l >= timer_trigger_l)) {
        wfi_bit = false;
        csr[MIP] |= 0x80;
    } else
        csr[MIP] &= ~0x80;

    if (wfi_bit) {
        usleep(500);
        return 0;
    }

    uint32_t exception = 0;
    uint32_t rval = 0;
    uint32_t cycle = csr[CYCLE_L];

    if ((csr[MIP] & 0x80) && (csr[MIE] & 0x80) && (csr[MSTATUS] & 0x8)) {
        exception = EXC_TIMER_INTERRUPT;
    } else {
        for (int icount = 0; icount < num_instructions; icount++) {
            rval = 0;
            cycle++;

            uint32_t ir = bus->read(pc, &exception);
            if (exception) break;

            uint32_t rd = (ir >> 7) & 0x1f;

            switch (ir & 0x7f) {
                case 0x37:  // LUI (0b0110111)
                    rval = (ir & 0xfffff000);
                    break;
                case 0x17:  // AUIPC (0b0010111)
                    rval = pc + (ir & 0xfffff000);
                    break;
                case 0x6F: {  // JAL (0b1101111)
                    int32_t addr = ((ir & 0x80000000) >> 11) | ((ir & 0x7fe00000) >> 20) | ((ir & 0x00100000) >> 9) | ((ir & 0x000ff000));
                    if (addr & 0x00100000) addr |= 0xffe00000;  // Sign extension.
                    rval = pc + 4;
                    pc = pc + addr - 4;
                    break;
                }
                case 0x67: {  // JALR (0b1100111)
                    uint32_t imm = ir >> 20;
                    int32_t imm_se = imm | ((imm & 0x800) ? 0xfffff000 : 0);
                    rval = pc + 4;
                    pc = ((x[(ir >> 15) & 0x1f] + imm_se) & ~1) - 4;
                    break;
                }
                case 0x63: {  // Branch (0b1100011)
                    uint32_t imm = ((ir & 0xf00) >> 7) | ((ir & 0x7e000000) >> 20) | ((ir & 0x80) << 4) | ((ir >> 31) << 12);
                    if (imm & 0x1000) imm |= 0xffffe000;
                    int32_t rs1 = x[(ir >> 15) & 0x1f];
                    int32_t rs2 = x[(ir >> 20) & 0x1f];
                    imm = pc + imm - 4;
                    rd = 0;

                    switch ((ir >> 12) & 0x7) {
                        case 0:  // BEQ
                            if (rs1 == rs2) pc = imm;
                            break;
                        case 1:  // BNE
                            if (rs1 != rs2) pc = imm;
                            break;
                        case 4:  // BLT
                            if (rs1 < rs2) pc = imm;
                            break;
                        case 5:  // BGE
                            if (rs1 >= rs2) pc = imm;
                            break;  // BGE
                        case 6:
                            if ((uint32_t)rs1 < (uint32_t)rs2) pc = imm;
                            break;  // BLTU
                        case 7:
                            if ((uint32_t)rs1 >= (uint32_t)rs2) pc = imm;
                            break;  // BGEU
                        default:
                            exception = 2;  // Invalid opcode
                    }
                    break;
                }
                case 0x03: {  // Load (0b0000011)
                    uint32_t rs1 = x[(ir >> 15) & 0x1f];
                    uint32_t imm = ir >> 20;
                    int32_t imm_se = imm | ((imm & 0x800) ? 0xfffff000 : 0);
                    uint32_t addr = rs1 + imm_se;

                    switch ((ir >> 12) & 0x7) {
                        case 0:  // LB
                            rval = (int8_t)bus->read(addr, &exception);
                            break;
                        case 1:  // LH
                            rval = (int16_t)bus->read(addr, &exception);
                            break;
                        case 2:  // LW
                            rval = bus->read(addr, &exception);
                            break;
                        case 4:  // LBU
                            rval = (uint8_t)bus->read(addr, &exception);
                            break;
                        case 5:  // LHU
                            rval = (uint16_t)bus->read(addr, &exception);
                            break;
                        default:
                            exception = 2;  // Invalid opcode
                    }
                    break;
                }
                case 0x23: {  // Store 0b0100011
                    uint32_t rs1 = x[(ir >> 15) & 0x1f];
                    uint32_t rs2 = x[(ir >> 20) & 0x1f];
                    uint32_t addr = ((ir >> 7) & 0x1f) | ((ir & 0xfe000000) >> 20);

                    if (addr & 0x800) addr |= 0xfffff000;
                    addr += rs1;
                    rd = 0;

                    switch ((ir >> 12) & 0x7) {
                        case 0:  // SB
                            exception = bus->write(addr, rs2, 1);
                            break;
                        case 1:  // SH
                            exception = bus->write(addr, rs2, 2);
                            break;
                        case 2:  // SW
                            exception = bus->write(addr, rs2, 4);
                            break;
                        default:
                            exception = 2;  // Invalid opcode
                    }
                    break;
                }
                case 0x13:    // Op-immediate 0b0010011
                case 0x33: {  // Op           0b0110011
                    uint32_t imm = ir >> 20;
                    imm = imm | ((imm & 0x800) ? 0xfffff000 : 0);
                    uint32_t rs1 = x[(ir >> 15) & 0x1f];
                    uint32_t is_reg = !!(ir & 0x20);
                    uint32_t rs2 = is_reg ? x[imm & 0x1f] : imm;

                    if (is_reg && (ir & 0x02000000)) {
                        switch ((ir >> 12) & 7) {  // 0x02000000 = RV32M
                            case 0:
                                rval = rs1 * rs2;
                                break;  // MUL
                            case 1:
                                rval = ((int64_t)((int32_t)rs1) * (int64_t)((int32_t)rs2)) >> 32;
                                break;  // MULH
                            case 2:
                                rval = ((int64_t)((int32_t)rs1) * (uint64_t)rs2) >> 32;
                                break;  // MULHSU
                            case 3:
                                rval = ((uint64_t)rs1 * (uint64_t)rs2) >> 32;
                                break;  // MULHU
                            case 4:
                                if (rs2 == 0)
                                    rval = -1;
                                else
                                    rval = ((int32_t)rs1 == INT32_MIN && (int32_t)rs2 == -1) ? rs1 : ((int32_t)rs1 / (int32_t)rs2);
                                break;  // DIV
                            case 5:
                                if (rs2 == 0)
                                    rval = 0xffffffff;
                                else
                                    rval = rs1 / rs2;
                                break;  // DIVU
                            case 6:
                                if (rs2 == 0)
                                    rval = rs1;
                                else
                                    rval = ((int32_t)rs1 == INT32_MIN && (int32_t)rs2 == -1) ? 0 : ((uint32_t)((int32_t)rs1 % (int32_t)rs2));
                                break;  // REM
                            case 7:
                                if (rs2 == 0)
                                    rval = rs1;
                                else
                                    rval = rs1 % rs2;
                                break;  // REMU
                        }
                    } else {
                        switch ((ir >> 12) & 7) {
                            case 0:  // ADD & SUB
                                rval = (is_reg && (ir & 0x40000000)) ? (rs1 - rs2) : (rs1 + rs2);
                                break;
                            case 1:  // SLL
                                rval = rs1 << (rs2 & 0x1F);
                                break;
                            case 2:  // SLT
                                rval = (int32_t)rs1 < (int32_t)rs2;
                                break;
                            case 3:  // SLTU
                                rval = rs1 < rs2;
                                break;
                            case 4:  // XOR
                                rval = rs1 ^ rs2;
                                break;
                            case 5:  // SRL & SRA
                                rval = (ir & 0x40000000) ? (((int32_t)rs1) >> (rs2 & 0x1F)) : (rs1 >> (rs2 & 0x1F));
                                break;
                            case 6:  // OR
                                rval = rs1 | rs2;
                                break;
                            case 7:  // AND
                                rval = rs1 & rs2;
                                break;
                        }
                    }
                    break;
                }
                case 0x0f:  // Fence 0b0001111
                    rd = 0;
                    break;
                case 0x73: {  // Zicsr & System 0b1110011
                    uint32_t csr_num = ir >> 20;
                    uint32_t micro_op = (ir >> 12) & 0x7;

                    if ((micro_op & 3)) {  // Zicsr
                        int rs1imm = (ir >> 15) & 0x1f;
                        uint32_t rs1 = x[rs1imm];
                        uint32_t writeval = rs1;
                        rval = csr[csr_num];

                        switch (micro_op) {
                            case 1:
                                writeval = rs1;
                                break;  // CSRRW
                            case 2:
                                writeval = rval | rs1;
                                break;  // CSRRS
                            case 3:
                                writeval = rval & ~rs1;
                                break;  // CSRRC
                            case 5:
                                writeval = rs1imm;
                                break;  // CSRRWI
                            case 6:
                                writeval = rval | rs1imm;
                                break;  // CSRRSI
                            case 7:
                                writeval = rval & ~rs1imm;
                                break;  // CSRRCI
                        }
                        csr[csr_num] = writeval;
                    } else if (micro_op == 0x0)  // System instruction
                    {
                        rd = 0;
                        if (csr_num == 0x105) {   // WFI
                            csr[MSTATUS] |= 0x8;  // Enable interrupts
                            wfi_bit = true;
                            pc += 4;
                            return 0;
                        } else if (((csr_num & 0xff) == 0x02)) {  // MRET & URET & SRET
                            uint32_t old_mstatus = csr[MSTATUS];
                            uint32_t old_op_mode = op_mode;
                            csr[MSTATUS] = ((old_mstatus & 0x80) >> 4) | (old_op_mode << 11) | 0x80;
                            op_mode = (old_mstatus >> 11) & 3;
                            pc = csr[MEPC] - 4;
                        } else {
                            switch (csr_num) {
                                case 0:
                                    exception = (op_mode) ? 11 : 8;
                                    break;  // ECALL
                                case 1:
                                    exception = 3;
                                    break;  // EBREAK 3
                                default:
                                    exception = 2;
                                    break;  // Invalid opcode
                            }
                        }
                    } else
                        exception = 2;  // Invalid opcode
                    break;
                }
                case 0x2f: {  // RV32A (0b00101111)
                    uint32_t rs1 = x[(ir >> 15) & 0x1f];
                    uint32_t rs2 = x[(ir >> 20) & 0x1f];
                    uint32_t irmid = (ir >> 27) & 0x1f;

                    rval = bus->read(rs1, &exception);
                    if (exception) break;

                    uint32_t dowrite = 1;
                    switch (irmid) {
                        case 2:  // LR.W
                            dowrite = 0;
                            reservation_addr = rs1;  // Set reservation address
                            break;
                        case 3:                                // SC.W
                            rval = (reservation_addr != rs1);  // Check reservation address and only write if it matches
                            dowrite = !rval;
                            break;
                        case 1:
                            break;  // AMOSWAP.W
                        case 0:
                            rs2 += rval;
                            break;  // AMOADD.W
                        case 4:
                            rs2 ^= rval;
                            break;  // AMOXOR.W
                        case 12:
                            rs2 &= rval;
                            break;  // AMOAND.W
                        case 8:
                            rs2 |= rval;
                            break;  // AMOOR.W
                        case 16:
                            rs2 = ((int32_t)rs2 < (int32_t)rval) ? rs2 : rval;
                            break;  // AMOMIN.W
                        case 20:
                            rs2 = ((int32_t)rs2 > (int32_t)rval) ? rs2 : rval;
                            break;  // AMOMAX.W
                        case 24:
                            rs2 = (rs2 < rval) ? rs2 : rval;
                            break;  // AMOMINU.W
                        case 28:
                            rs2 = (rs2 > rval) ? rs2 : rval;
                            break;  // AMOMAXU.W
                        default:
                            exception = 2;
                            dowrite = 0;
                            break;
                    }
                    if (dowrite) bus->write(rs1, rs2, 4);
                    break;
                }
                default:
                    exception = 2;  // Invalid opcode
            }

            if (reset_triggered) return 1;  // If reset triggered, break out of loop
            if (exception) break;           // If exception, break out of loop
            if (rd) x[rd] = rval;           // If rd is set, write the return value to the register
            pc += 4;                        // Increment the program counter
        }
    }

    if (exception) {                   // Handle exceptions
        if (exception & 0x80000000) {  // Handle an Interrupt (MSB set)
            csr[MTVAL] = 0;
        } else {
            csr[MTVAL] = (exception > 4 && exception <= 7) ? rval : pc;
        }

        csr[MCAUSE] = exception;                                        // Store the exception cause
        csr[MEPC] = pc;                                                 // Store the program counter
        csr[MSTATUS] = ((csr[MSTATUS] & 0x08) << 4) | (op_mode << 11);  // Store the status register
        pc = csr[MTVEC];                                                // Set the program counter to the exception handler address
        op_mode = OPMODE_MACHINE;                                       // Set the operation mode to machine mode
    }

    if (csr[CYCLE_L] > cycle) csr[CYCLE_H]++;  // Increment the cycle high register if the low register has overflowed
    csr[CYCLE_L] = cycle;                      // Set the cycle low register to the current cycle count
    return 0;
}

/**
 * Get the low 32 bits of the timer.
 * @return The low 32 bits of the timer.
 */
uint32_t Cpu::getTimerL() {
    return timer_l;
}

/**
 * Get the high 32 bits of the timer.
 * @return The high 32 bits of the timer.
 */
uint32_t Cpu::getTimerH() {
    return timer_h;
}

/**
 * Set the high 32 bits of the timer trigger.
 * @param value The value to set.
 */
void Cpu::setTimerTriggerH(uint32_t value) {
    timer_trigger_h = value;
}

/**
 * Set the low 32 bits of the timer trigger.
 * @param value The value to set.
 */
void Cpu::setTimerTriggerL(uint32_t value) {
    timer_trigger_l = value;
}