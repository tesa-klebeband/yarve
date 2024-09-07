# yarve
YetAnotherRiscVEmulator is an easily extensible RV32ima emulator capable of running linux

## Building
Install the following dependencies by running the command suitable for your package manager:
- Debian/Ubuntu:
```bash
sudo apt update && sudo apt install -y git g++ make
```
- Fedora:
```bash
sudo dnf install -y git gcc-c++ make
```
- Arch:
```bash
sudo pacman -Syu --needed git gcc make
```

Then clone the repository and build the emulator:
```bash
git clone https://github.com/tesa-klebeband/yarve.git
cd yarve
make
```

## Building Linux
To build the linux kernel, install the following dependencies by running the command suitable for your package manager:
- Debian/Ubuntu:
```bash
sudo apt update && sudo apt install -y which sed make binutils build-essential diffutils gcc g++ bash patch gzip bzip2 perl tar cpio unzip rsync file bc findutils wget
```
- Fedora:
```bash
sudo dnf install -y which sed make binutils gcc gcc-c++ bash patch gzip bzip2 perl tar cpio unzip rsync file bc findutils wget diffutils
```
- Arch:
```bash
sudo pacman -Syu --needed which sed make binutils base-devel diffutils gcc bash patch gzip bzip2 perl tar cpio unzip rsync file bc findutils wget
```
Build the kernel by issuing the following command from the root of the repository:
```bash
make linux
```
To test the emulator with the linux kernel, run the following command:
```bash
make run-linux
```

## Modifying buildroot and linux configurations
The buildroot configuration contains all the settings for the root filesystem and the linux configuration contains all the settings for the kernel. Both configurations can be modified by running the following commands:
```bash
cd linux && make edit-buildroot-config
cd linux && make edit-linux-config
```

## Extending the emulator
yarve is designed to be easily extensible. Adding a new memory mapped device is as simple as creating a new class (preferably in a new file) that inherits from `BusDevice` class and implementing the virtual functions. The new device can then be attached to the bus by calling bus->attach() in `src/riscv.cpp`.
Modifying cpu fields from a device can be achieved by using the `ICPUInterface` interface. An example of this can be found in `src/syscon.cpp`.

## License
All files within this repo are released under the GNU GPL V3 License as per the LICENSE file stored in the root of this repo.