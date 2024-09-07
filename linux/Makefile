BUILDROOT_TAR = buildroot-2024.08-rc3.tar.xz
BUILDROOT_URL = https://buildroot.org/downloads/$(BUILDROOT_TAR)
BUILDROOT_DIR = buildroot
DOWNLOAD_DIR = dl
CONFIG_DIR = configs
BUILDROOT_CONFIG = $(CONFIG_DIR)/buildroot_config
LINUX_CONFIG = $(CONFIG_DIR)/yarve_defconfig
BUILD_DIR = build
OBJCOPY = $(BUILDROOT_DIR)/output/host/bin/riscv32-buildroot-linux-uclibc-objcopy
DTC = $(BUILDROOT_DIR)/output/host/bin/dtc
DTB_BASENAME = yarve
KERNEL_BIN = $(BUILD_DIR)/Image

all: prepare linux

prepare:
	@mkdir -p $(BUILDROOT_DIR)
	@mkdir -p $(DOWNLOAD_DIR)
	@mkdir -p $(BUILD_DIR)
	@echo "Downloading Buildroot..."
	wget -c $(BUILDROOT_URL) -O $(DOWNLOAD_DIR)/$(BUILDROOT_TAR)
	@echo "Extracting Buildroot..."
	tar -xf $(DOWNLOAD_DIR)/$(BUILDROOT_TAR) -C $(BUILDROOT_DIR) --strip-components=1

linux:
	@echo "Building Linux..."
	@cp $(BUILDROOT_CONFIG) $(BUILDROOT_DIR)/.config
	@echo "Entering Buildroot directory..."
	@cd $(BUILDROOT_DIR) && make
	@echo "Leaving Buildroot directory..."
	@cd ..
	make dtb
	@echo "Stripping Linux image..."
	$(OBJCOPY) -S -O binary $(BUILDROOT_DIR)/output/images/vmlinux $(KERNEL_BIN)
	@echo "Done! Output files are stored in '$(BUILD_DIR)'."

dtb:
	@echo "Building DTB..."
	$(DTC) -O dtb -o $(BUILD_DIR)/$(DTB_BASENAME).dtb $(CONFIG_DIR)/yarve.dts

edit-buildroot-config:
	@cp $(BUILDROOT_CONFIG) $(BUILDROOT_DIR)/.config
	@cd $(BUILDROOT_DIR) && make menuconfig
	@cp $(BUILDROOT_DIR)/.config $(BUILDROOT_CONFIG)
	@echo "Buildroot configuration in '$(BUILDROOT_CONFIG)' has been updated."

edit-linux-config:
	@cp $(LINUX_CONFIG) $(BUILDROOT_DIR)/output/build/linux-*/.config
	@cd $(BUILDROOT_DIR) && make linux-menuconfig
	@cp $(BUILDROOT_DIR)/output/build/linux-*/.config $(LINUX_CONFIG)
	@echo "Linux configuration in '$(LINUX_CONFIG)' has been updated."

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(DOWNLOAD_DIR)
	@rm -rf $(BUILDROOT_DIR)