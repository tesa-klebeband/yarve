#include "clint.h"

/**
 * Construct a new CLINT device.
 * @param cpu The CPU interface.
 * @param base The base address of the CLINT device.
 * @param size The size of the CLINT device.
 */
Clint::Clint(ICpuInterface *cpu, uint32_t base, size_t size) {
    this->cpu = cpu;
    this->base = base;
    this->size = size;
}

/**
 * Get the device information.
 * @return The device information.
 */
DeviceInfo Clint::getDeviceInfo() {
    DeviceInfo info;
    info.base = base;
    info.size = size;
    info.device = this;
    return info;
}

/**
 * Read a word from the CLINT device.
 * @param addr The address to read from.
 * @return The word read from the CLINT device.
 */
uint32_t Clint::read(uint32_t addr) {
    if (addr == (base + 0xbff8)) {
        return cpu->getTimerL();
    } else if (addr == (base + 0xbffc)) {
        return cpu->getTimerH();
    }
    return 0;
}

/**
 * Write a word to the CLINT device.
 * @param addr The address to write to.
 * @param data The data to write.
 * @param width The width of the data to write.
 */
void Clint::write(uint32_t addr, uint32_t data, uint8_t width) {
    if (addr == (base + 0x4000)) {
        cpu->setTimerTriggerL(data);
    } else if (addr == (base + 0x4004)) {
        cpu->setTimerTriggerH(data);
    }
}