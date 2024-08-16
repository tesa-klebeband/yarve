#include "bus.h"

/**
 * Construct a Bus object and initialize the devices vector.
*/
Bus::Bus() {
    devices.clear();
}

/**
 * Attach a device to the bus.
 * @param device The device to attach.
 * @param base The base address of the device.
 * @param size The size of the device.
*/
void Bus::attach(BusDevice* device) {
    DeviceInfo info = device->getDeviceInfo();
    devices.push_back(info);
}

uint32_t Bus::read(uint32_t addr, uint32_t *exception) {
    for (auto& device : devices) {
        if (addr >= device.base && addr < device.base + device.size) {
            *exception = BUS_READ_OK;
            return device.device->read(addr);
        }
    }
    *exception = BUS_READ_ERROR;
    return 0;
}

/**
 * Write a byte to the bus.
 * @param addr The address to write to.
 * @param data The data to write.
 * @return A defined status in bus.h.
*/
uint32_t Bus::write(uint32_t addr, uint32_t data, uint8_t width) {
    for (auto& device : devices) {
        if (addr >= device.base && addr < device.base + device.size) {
            device.device->write(addr, data, width);
            return BUS_WRITE_OK;
        }
    }
    return BUS_WRITE_ERROR;
}