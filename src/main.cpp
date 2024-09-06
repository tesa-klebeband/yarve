#include <iostream>
#include <string>
#include "riscv.h"

void printHelp(std::string exec_name) {
    std::cout << "Usage: " << exec_name << " [OPTION]... -b [KERNEL]" << std::endl;
    std::cout << "Emulates a RiscV machine." << std::endl;
    std::cout << "All addresses have to be specified in hexadecimal, starting with '0x'." << std::endl;
    std::cout << std::endl;
    std::cout << "  -h, --help        display this help and exit" << std::endl;
    std::cout << "  -v, --version     display version information and exit" << std::endl;
    std::cout << "  -r, --ram         specify the RAM size in bytes" << std::endl;
    std::cout << "  -R, --ram-base    specify the base address of the RAM" << std::endl;
    std::cout << "  -d, --dtb         specify the device tree blob file" << std::endl;
    std::cout << "  -D, --dtb-base    specify the base address of the device tree blob" << std::endl;
    std::cout << "  -k, --kernel      specify the kernel to load" << std::endl;
    std::cout << "  -K, --kernel-base specify the base address of the kernel" << std::endl;
    std::cout << "  -e, --entry       specify the entry point of the kernel" << std::endl;
    std::cout << std::endl << std::flush;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp(argv[0]);
        return 1;
    }

    RiscV riscv;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            printHelp(argv[0]);
            return 0;
        } else if ((arg == "-v") || (arg == "--version")) {
            std::cout << "yarve (YetAnotherRiscVEmulator) v0.1.0" << std::endl;
            std::cout << "Copyrigth (C) 2024 tesa-klebeband" << std::endl;
            std::cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>." << std::endl << std::flush;
            return 0;
        } else if ((arg == "-r") || (arg == "--ram")) {
            riscv.ram_size = std::stoul(argv[++i]);
        } else if ((arg == "-R") || (arg == "--ram-base")) {
            riscv.ram_base = std::stoul(argv[++i], nullptr, 16);
        } else if ((arg == "-d") || (arg == "--dtb")) {
            riscv.dtb_file = argv[++i];
        } else if ((arg == "-D") || (arg == "--dtb-base")) {
            riscv.dtb_base = std::stoul(argv[++i], nullptr, 16);
        } else if ((arg == "-k") || (arg == "--kernel")) {
            riscv.kernel_file = argv[++i];
        } else if ((arg == "-K") || (arg == "--kernel-base")) {
            riscv.kernel_base = std::stoul(argv[++i], nullptr, 16);
        } else if ((arg == "-e") || (arg == "--entry")) {
            riscv.kernel_entry = std::stoul(argv[++i], nullptr, 16);
        } else {
            std::cout << "yarve: unrecognized option '" << arg << "'" << std::endl;
            std::cout << "Try 'yarve --help' for more information." << std::endl << std::flush;
            return 1;
        }
    }

    if (riscv.kernel_file == "") {
        std::cout << "yarve: no kernel file specified" << std::endl;
        std::cout << "Try 'yarve --help' for more information." << std::endl << std::flush;
        return 1;
    }

    if (riscv.dtb_file == "") {
        std::cout << "Warning: no device tree blob file specified" << std::endl << std::flush;
    }

    riscv.initialize();
    riscv.run();

    return 0;
}   