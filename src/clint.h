#ifndef CLINT_H
#define CLINT_H

#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "bus.h"

#define DEFAULT_CLINT_BASE 0x11000000
#define DEFAULT_CLINT_SIZE 0x10000

class Clint : public BusDevice {
    public:
        Clint(ICpuInterface *cpu, uint32_t base = DEFAULT_CLINT_BASE, size_t size = DEFAULT_CLINT_SIZE);
        DeviceInfo getDeviceInfo();
        uint32_t read(uint32_t addr);
        void write(uint32_t addr, uint32_t data, uint8_t width);

    private:
        uint32_t base;
        size_t size;
        ICpuInterface *cpu;
};

#endif