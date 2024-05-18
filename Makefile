TOOLCHAIN=~/AbdooOwd/Toolchain/i686-elf-bin

QEMU=qemu-system-i386

ASM=nasm
CC16=$(TOOLCHAIN)/i686-elf-gcc
LD16=$(TOOLCHAIN)/i686-elf-ld

C_FLAGS=-ffreestanding -c

ROOTFS_DIR=rootfs
BUILD_DIR=bin
SRC_DIR=src

OS_FILENAME=OS.bin


# functions ?
rwildcard = $(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

.PHONY: all always os-image run clear

all: os-image run
build: os-image

os-image: $(BUILD_DIR)/$(OS_FILENAME)
bootloader: stage1 stage2

stage1: $(BUILD_DIR)/stage1.bin
stage2: $(BUILD_DIR)/stage2.bin

$(BUILD_DIR)/$(OS_FILENAME): always bootloader
	dd if=/dev/zero of=$@ bs=512 count=2880
	mkfs.fat -F 12 -n "ABOS" $@
	dd if=$(BUILD_DIR)/stage1.bin of=$@ bs=512 count=1 conv=notrunc
	mcopy -i $@ $(BUILD_DIR)/stage2.bin "::STAGE2.SYS"
	mcopy -i $@ $(ROOTFS_DIR)/test.txt	"::test.txt"


$(BUILD_DIR)/stage1.bin: $(SRC_DIR)/boot/stage1.asm
	$(ASM) $< -f bin -o $@

$(BUILD_DIR)/stage2.bin: $(SRC_DIR)/boot/stage2.asm
	$(ASM) $< -f bin -o $@


# Processes

run: os-image
	$(QEMU) -fda $(BUILD_DIR)/$(OS_FILENAME)

# Organization stuff

always:
	mkdir -p $(BUILD_DIR)

clear clean:
	rm -rf $(BUILD_DIR)/*