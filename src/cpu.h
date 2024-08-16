#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "bus.h"

#define DEFAULT_CPU_PC 0x80000000
#define DEFAULT_DTB_BASE 0x87F00000

#define EXC_INSTRUCTION_ACCESS_FAULT 1
#define EXC_ILLEGAL_INSTRUCTION 2
#define EXC_EBREAK 3
#define EXC_LOAD_ACCESS_FAULT 5
#define EXC_STORE_ACCESS_FAULT 7
#define EXC_ECALL_U_MODE 8
#define EXC_ECALL_M_MODE 11
#define EXC_TIMER_INTERRUPT 0x80000007

#define MSTATUS 0x300
#define CYCLE_L 0xC00
#define CYCLE_H 0xC80
#define MSCRATCH 0x340
#define MTVEC 0x305
#define MEPC 0x341
#define MCAUSE 0x342
#define MTVAL 0x343
#define MIP 0x344
#define MIE 0x304
#define MISA 0x301
#define MVENDORID 0xF11

#define OPMODE_USER 0
#define OPMODE_MACHINE 3

class ICpuInterface {
    public:
        virtual uint32_t getTimerL() = 0;
        virtual uint32_t getTimerH() = 0;
        virtual void setTimerTriggerH(uint32_t value) = 0;
        virtual void setTimerTriggerL(uint32_t value) = 0;
};

class Cpu : public ICpuInterface {
    public:
        Cpu(Bus* bus);
        void reset(uint32_t program_counter = DEFAULT_CPU_PC, uint32_t dtb_base = DEFAULT_DTB_BASE);
        void execute(uint32_t num_instructions, uint32_t elapsed_micros);
        uint32_t getTimerL();
        uint32_t getTimerH();
        void setTimerTriggerH(uint32_t value);
        void setTimerTriggerL(uint32_t value);

    private:
        uint32_t pc;
        uint32_t x[32];
        uint32_t csr[4096];
        uint32_t load_reservation;
        uint8_t operation_mode;
        uint32_t timer_l;
        uint32_t timer_h;
        uint32_t timer_trigger_l;
        uint32_t timer_trigger_h;
        uint32_t reservation_addr;
        uint8_t op_mode;
        bool wfi_bit;
        Bus* bus;
};

#endif