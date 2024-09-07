#ifndef SYSCON_H
#define SYSCON_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "bus.h"
#include "cpu.h"

#define DEFAULT_SYSCON_BASE 0x11100000
#define DEFAULT_SYSCON_SIZE 0x100
#define SYSCON_POWEROFF 0x1
#define SYSCON_REBOOT 0x2

class Syscon : public BusDevice {
    public:
        Syscon(ICpuInterface *cpu, uint32_t base = DEFAULT_SYSCON_BASE, size_t size = DEFAULT_SYSCON_SIZE);
        DeviceInfo getDeviceInfo();
        uint32_t read(uint32_t addr);
        void write(uint32_t addr, uint32_t data, uint8_t width);

    private:
        uint32_t base;
        size_t size;
        ICpuInterface *cpu;
};

#endif