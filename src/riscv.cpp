#include "riscv.h"

/**
 * Construct a new RiscV machine.
*/
RiscV::RiscV() {
}

/**
 * Initialize the RiscV machine.
*/
void RiscV::initialize() {
    bus = new Bus();
    ram = new Ram(ram_base, ram_size);
    cpu = new Cpu(bus);
    uart = new Uart();
    clint = new Clint(cpu);
    bus->attach(ram);
    bus->attach(uart);
    bus->attach(clint);

    ram->loadBinary(binary_file.c_str(), binary_base);
    if (dtb_file != "") ram->loadBinary(dtb_file.c_str(), dtb_base);
}

/**
 * Run the RiscV machine.
*/
void RiscV::run() {
    cpu->reset(binary_entry, dtb_base);
    
    timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t last_update = tv.tv_usec;

    while (true) {
        gettimeofday(&tv, NULL);
        uint32_t elapsed = tv.tv_usec - last_update;
        if ((int32_t)elapsed < 0) elapsed = 0;
        last_update = tv.tv_usec;
        cpu->execute(1024, elapsed);
    }
}