#ifndef BUS_H
#define BUS_H

#include <stdint.h>
#include <vector>

#define BUS_READ_OK 0
#define BUS_READ_ERROR 5
#define BUS_WRITE_OK 0
#define BUS_WRITE_ERROR 7

class BusDevice;

typedef struct {
    BusDevice* device;
    uint32_t base;
    size_t size;
} DeviceInfo;

class BusDevice {
    public:
        virtual DeviceInfo getDeviceInfo() = 0;
        virtual uint32_t read(uint32_t addr) = 0;
        virtual void write(uint32_t addr, uint32_t data, uint8_t width) = 0;
};

class Bus {
    public:
        Bus();
        void attach(BusDevice* device);
        uint32_t read(uint32_t addr, uint32_t *exception);
        uint32_t write(uint32_t addr, uint32_t data, uint8_t width);

    private:
        std::vector<DeviceInfo> devices;
};

#endif