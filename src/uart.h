#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include "bus.h"

#define DEFAULT_UART_BASE 0x10000000
#define DEFAULT_UART_SIZE 0x8

class Uart : public BusDevice {
    public:
        Uart(uint32_t base = DEFAULT_UART_BASE, size_t size = DEFAULT_UART_SIZE);
        DeviceInfo getDeviceInfo();
        uint32_t read(uint32_t addr);
        void write(uint32_t addr, uint32_t data, uint8_t width);

    private:
        uint32_t base;
        size_t size;
        uint32_t readStdin();
        bool checkStdin();
};

#endif