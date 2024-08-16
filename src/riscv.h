#ifndef RISC_V_H
#define RISC_V_H

#include <stdint.h>
#include <sys/time.h>
#include <string>
#include <unistd.h>
#include "cpu.h"
#include "bus.h"
#include "ram.h"
#include "uart.h"
#include "clint.h"

class RiscV {
    public:
        RiscV();
        void initialize();
        void run();
        
        int ram_size = DEFAULT_RAM_SIZE;
        int ram_base = DEFAULT_RAM_BASE;
        int binary_base = DEFAULT_CPU_PC;
        int binary_entry = DEFAULT_CPU_PC;
        int dtb_base = DEFAULT_DTB_BASE;
        std::string binary_file;
        std::string dtb_file;

    private:
        Bus *bus;
        Ram *ram;
        Cpu *cpu;
        Uart *uart;
        Clint *clint;
};

#endif