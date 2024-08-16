#include "ram.h"

/**
 * Construct a new RAM device.
 * @param base The base address of the RAM device.
 * @param size The size of the RAM device.
 */
Ram::Ram(uint32_t base, size_t size) {
    this->base = base;
    this->size = size;
    data = new uint8_t[size];
    memset(data, 0, size);
}

/**
 * Get the device information.
 * @return The device information.
 */
DeviceInfo Ram::getDeviceInfo() {
    DeviceInfo info;
    info.base = base;
    info.size = size;
    info.device = this;
    return info;
}

/**
 * Load a binary file into memory.
 * @param filename The name of the file to load.
 * @param address The address to load the file at.
 */
void Ram::loadBinary(const char* filename, uint32_t address) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Could not open file %s\n", filename);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if ((int32_t)(address - base) < 0) {
        fprintf(stderr, "Error: Address %08X is out of bounds\n", address);
        exit(1);
    }
    if ((address - base + file_size) > size) {
        fprintf(stderr, "Error: File %s is too large to fit in RAM or was attempted to be loaded at an unsufficient address\n", filename);
        exit(1);
    }

    fread(data + address - base, 1, file_size, file);
    fclose(file);
}

/**
 * Read a word from the RAM device.
 * @param addr The address to read from.
 * @return The word read from the RAM device.
 */
uint32_t Ram::read(uint32_t addr) {
    return *(uint32_t*)(data + addr - base);
}

/**
 * Write a word of varying width to the RAM device.
 * @param addr The address to write to.
 * @param data The data to write.
 * @param width The width of the data to write.
 */
void Ram::write(uint32_t addr, uint32_t data, uint8_t width) {
    switch (width) {
        case 1:
            *(uint8_t*)(this->data + addr - base) = data;
            break;
        case 2:
            *(uint16_t*)(this->data + addr - base) = data;
            break;
        case 4:
            *(uint32_t*)(this->data + addr - base) = data;
            break;
    }
}