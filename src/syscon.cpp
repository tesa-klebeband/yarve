#include "syscon.h"

/**
 * Construct a new Syscon device.
 * @param cpu The CPU device.
 * @param base The base address of the Syscon device.
 * @param size The size of the Syscon device.
 */
Syscon::Syscon(ICpuInterface *cpu, uint32_t base, size_t size) {
    this->cpu = cpu;
    this->base = base;
    this->size = size;
}

/**
 * Get the device information.
 * @return The device information.
 */
DeviceInfo Syscon::getDeviceInfo() {
    DeviceInfo info;
    info.base = base;
    info.size = size;
    info.device = this;
    return info;
};

/**
 * Read a word from the Syscon device.
 * @param addr The address to read from.
 * @return The word read from the Syscon device.
 */
uint32_t Syscon::read(uint32_t addr) {
    return 0;
}

/**
 * Write a word to the Syscon device.
 * @param addr The address to write to.
 * @param data The data to write.
 * @param width The width of the data to write.
 */
void Syscon::write(uint32_t addr, uint32_t data, uint8_t width) {
    if (addr == base) {
        if (data == SYSCON_POWEROFF) {
            exit(0);
        } else if (data == SYSCON_REBOOT) {
            cpu->triggerReset();
        }
    }
}