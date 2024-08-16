#include "uart.h"

uint32_t stdin_fcntl;
/**
 * Construct a new UART device and initialize it.
 * @param base The base address of the UART device.
 * @param size The size of the UART device.
 */
Uart::Uart(uint32_t base, size_t size) {
    this->base = base;
    this->size = size;

    struct termios term;
    tcgetattr(fileno(stdin), &term);
    term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &term);

    atexit([]() {
        struct termios term;
        tcgetattr(fileno(stdin), &term);
        term.c_lflag |= ICANON | ECHO;
        tcsetattr(fileno(stdin), TCSANOW, &term);
    });
}

/**
 * Get the device information.
 * @return The device information.
 */ 
DeviceInfo Uart::getDeviceInfo() {
    DeviceInfo info;
    info.base = base;
    info.size = size;
    info.device = this;
    return info;
}

/**
 * Read a word from the UART device.
 * @param addr The address to read from.
 * @return The word read from the UART device.
 */
uint32_t Uart::read(uint32_t addr) {
    if (addr == base + 0x5) {
        return 0x60 | checkStdin();
    } else if (addr == base) {
        return readStdin();
    }
    return 0;
}

/**
 * Write a word to the UART device.
 * @param addr The address to write to.
 * @param data The data to write.
 * @param width The width of the data to write.
 */
void Uart::write(uint32_t addr, uint32_t data, uint8_t width) {
    if (addr == base) {
        printf("%c", data);
        fflush(stdout);
    }
}

/**
 * Read from the standart input if available.
 * @return The read character.
*/
uint32_t Uart::readStdin() {
    int bytes_available;
    ioctl(0, FIONREAD, &bytes_available);
    if (bytes_available > 0) {
        char c;
        ::read(fileno(stdin), &c, 1);
        return c;
    } else {
        return 0;
    }
}

/**
 * Check if there is data available to read from the standart input.
 * @return 1 if there is data available, 0 otherwise.
 */
bool Uart::checkStdin() {
    int bytes_available;
    ioctl(0, FIONREAD, &bytes_available);
    return bytes_available > 0;
}