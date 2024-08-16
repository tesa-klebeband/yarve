#ifndef RAM_H
#define RAM_H

#define DEFAULT_RAM_BASE 0x80000000
#define DEFAULT_RAM_SIZE 128 * 1024 * 1024

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bus.h"

class Ram : public BusDevice {
    public:
        Ram(uint32_t base = DEFAULT_RAM_BASE, size_t size = DEFAULT_RAM_SIZE);
        void loadBinary(const char* filename, uint32_t address);
        DeviceInfo getDeviceInfo();
        uint32_t read(uint32_t addr);
        void write(uint32_t addr, uint32_t data, uint8_t width);

    private:
        uint32_t base;
        size_t size;
        uint8_t *data;
};  

#endif